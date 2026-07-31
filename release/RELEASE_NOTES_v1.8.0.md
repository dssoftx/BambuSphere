# BambuSphere v1.8.0 (beta)

Feature release on top of v1.7.2: every configured printer now stays
connected in the background at all times, so switching the active printer
on-screen is instant instead of waiting on a fresh reconnect. Paired with a
round of performance work driven by the same change, since more concurrent
connections meant internal RAM and CPU headroom mattered more than before.
Published as a beta while this gets more real-world testing across
different printer counts and network conditions.

## Release Scope

- **Base**: v1.7.2.
- **Toolchain target**: ESP-IDF v5.5.4, LVGL v9.5.0.
- **Hardware variants**: AMOLED 1.75, mainline (`all`) and `h2x2d` printer-
  target build. LCD 2.8C is unaffected by this release (still on v1.6.2).

## New: always-on multi-printer background connections

Previously, `Application` held a single local `PrinterClient` and a single
`BambuCloudClient`, both reconfigured for whichever printer was "active".
Switching printers tore down and rebuilt the local MQTT+TLS session and the
cloud binding from scratch every time, which is where the reported 1-2
minute "settle in" delay came from.

- **Local MQTT** (`PrinterClientPool`, new `printer_client_pool.cpp`): every
  printer profile with local LAN config now gets its own persistent
  `PrinterClient` from boot onward. Switching the active printer no longer
  touches the local connection at all — it just reads an already-live
  connection. The printer-select cards also now show each printer's own
  live connection state instead of only the active one.
- **Bambu Cloud** (`bambu_cloud_client.cpp`): the single account-level cloud
  MQTT session now subscribes to every cloud-bound printer's report topic,
  not just the active one — mirroring how Bambu's own apps use the
  account-level session. Switching the active printer (`set_active_serial`)
  re-points REST/command targeting without resubscribing or reconnecting.
- Camera streaming and cloud REST preview/task-history polling both stay
  scoped to the active printer only (on-demand, as before) — always-on
  background connections only applies to the lightweight status/telemetry
  path, not video or REST polling.

### Fixes found during live testing of the above

- **Cloud session dropping ~5s after every switch**: the periodic Bambu
  Cloud device-binding refresh compared the newly active serial against a
  cleared `resolved_serial_` and treated it as "the bound printer changed",
  tearing down and rebuilding the *entire* cloud session (dropping every
  background printer's subscription, not just the active one) shortly after
  every switch. Fixed by setting `resolved_serial_` to match immediately on
  switch instead of clearing it.
- **Random audio chimes when switching printers**: the audio notifier's
  print-lifecycle edge-detector tracked one app-wide "last state" baseline
  rather than one per printer. Switching to a printer with a different
  print state (e.g. printing vs. idle) read as a real lifecycle transition
  of "the" printer and fired a chime. Fixed by re-priming the baseline
  silently whenever the active printer index changes, regardless of
  whether the switch came from the touchscreen or the web Setup Portal.

## Performance

Prompted by the always-on connections above needing more internal RAM/CPU
headroom, but these apply regardless of printer count:

- Moved the remaining long-lived internal-RAM task stacks to PSRAM
  (`audio_notif`, the Improv-serial provisioning task, OTA/reboot helper
  tasks) — internal SRAM is the scarce, contended resource shared with
  Wi-Fi/TLS/LVGL.
- Disabled `CONFIG_MBEDTLS_DEBUG`: it was compiled in at verbose level and
  emitting a trace line for every TLS record on every connection,
  regardless of the runtime `esp_log_level_set()` call meant to suppress it
  — confirmed live via serial capture. With every configured printer's
  local MQTT plus cloud MQTT now concurrent TLS connections, this scaled
  with printer count.
- Downgraded high-frequency per-report diagnostic logging
  (`printer_client.cpp`, `bambu_cloud_client.cpp`, `ui.cpp`) from `ESP_LOGI`
  to `ESP_LOGD`, which this project's `CONFIG_LOG_MAXIMUM_LEVEL=3` compiles
  out entirely — these fired on every MQTT report from every connected
  printer.
- Lowered the LVGL display refresh rate from 60 Hz to 30 Hz
  (`CONFIG_LV_DEF_REFR_PERIOD`): roughly halves redraw/compositing load
  with no meaningful perceived difference for a status/monitoring UI.

## Known Notes

- Tested live on a physical device this cycle across multiple local
  printers with mixed AMS/camera configurations, including repeated
  switching, but not yet across a wide range of printer counts, models or
  network conditions.
- There's a known, separate, pre-existing (not introduced by this release)
  occasional ~70-90ms UI stall when switching to a printer whose AMS unit
  count or camera availability differs from the previous one — a large-area
  screen redraw, not a continuous cost. Root cause not fully pinned down;
  a fix attempt during this cycle didn't measurably help and was reverted
  rather than ship unverified.
- If you have many printer profiles configured, local MQTT now opens one
  socket per printer with local config, concurrently, for the app's entire
  runtime — `PrinterClientPool` logs a warning past ~8 simultaneously
  connected printers, since `LWIP_MAX_SOCKETS=16` is shared with cloud
  MQTT, the Setup Portal HTTP server, OTA and Improv-serial.
