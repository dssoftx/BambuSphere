# BambuSphere v1.7.2 (beta)

Bugfix release on top of v1.7.1, focused on issues reported on the H2
family and X2D. Published as a beta/unstable build while these fixes get
more real-world testing on affected hardware. This release also introduces
a dedicated build variant for the H2 family/X2D — see below.

## Release Scope

- **Base**: v1.7.1.
- **Toolchain target**: ESP-IDF v5.5.4, LVGL v9.5.0.
- **Hardware variants**: both AMOLED 1.75 and LCD 2.8C, each available in
  the mainline (`all`) and new `h2x2d` printer-target build (see below).

## New: dedicated H2/X2D build variant

Enabling Developer Mode on the H2 family/X2D — required for local status on
these models — also disables the printer's own Bambu Cloud connection. On
this printer family, Hybrid mode's usual promise (local speed with cloud as
a safety net) doesn't hold: it's local-with-Developer-Mode or
cloud-only-without-Developer-Mode, never genuinely both.

A new `h2x2d` build (`PRINTSPHERE_PRINTER_TARGET=h2x2d`, see
[docs/Build/README.md](../docs/Build/README.md)) targets this directly:

- Connection Mode defaults to **Local Only** on a fresh config. Cloud and
  Hybrid remain fully selectable in Web Config afterward — this only
  changes the out-of-the-box default.
- Web Config shows a warning notice covering known rough edges on this
  printer family: random MQTT disconnects, the ESP32 running warm to the
  touch, and Hybrid/Cloud mode being slow without Developer Mode enabled on
  the printer.

The mainline build's behavior is unchanged for A1/P1/X1 users.

## Fixes

- **Right/secondary nozzle temperature freezing after boot (H2D and other
  dual-nozzle models)**: the inactive nozzle's temperature was read
  correctly exactly once — typically right after connecting — and then
  never updated again for the rest of the session, while the active
  nozzle kept updating normally. Root cause: the merge logic that picks
  the secondary nozzle's reading out of each MQTT status push only wrote a
  new value when the previously stored value was `<= 0`, which became
  permanently false after the first successful read. Both the local MQTT
  parser (`printer_client.cpp`) and the Bambu Cloud parser
  (`bambu_cloud_client.cpp`) had this bug; both are fixed. Confirmed root
  cause, not a mitigation.
- **Random disconnects with an audible chime (H2 family/X2D)**: this
  firmware does not itself play a connect/disconnect sound — the chime is
  the printer's own stock firmware reacting to a new local MQTT session.
  What BambuSphere controls is how often it forces that new session: the
  no-data watchdog that detects a stalled connection used a single
  30s-probe/15s-reconnect threshold for every printer. Models that require
  Developer Mode for local status (H2C/H2D/H2D Pro/H2S/X2D) now get a
  looser 60s/30s threshold, since their local broker has been observed
  reporting on a less consistent cadence than A1/P1/X1 firmware, which
  could trip the generic watchdog into avoidable reconnects. **This is a
  mitigation and a diagnostics improvement, not a confirmed fix** — build
  with a `-debug`-suffixed version (enables `PRINTSPHERE_DEBUG_BUILD`,
  e.g. `-DPRINTSPHERE_RELEASE_VERSION=v1.7.2-debug`) to view reconnect/
  watchdog activity live via Web Config's debug log viewer
  (`/api/debug/log`), which will show whether reconnects actually drop
  with this change or whether the disconnects are genuine Wi-Fi/RF issues
  outside this firmware's control.
- **Clock showing the wrong time since flash (all printers/boards)**: SNTP
  itself was always working correctly — the real issue was that time zone
  defaults to UTC and was never applied automatically. A fresh flash
  correctly synced to UTC and displayed correct UTC time, which reads as
  "the clock is broken" to anyone outside UTC until they happen to open
  Web Config and manually click Apply on the time zone dropdown. Web
  Config now auto-applies the browser-detected time zone on first load
  when none is saved yet, instead of only pre-filling the dropdown.
  Self-gating: once a zone is saved, this never re-fires. Also added an
  SNTP sync-notification log line (diagnostic only, no behavior change) to
  make future "what time did it actually sync at" questions easier to
  answer from the debug log.

## Known Notes

- The watchdog change for H2/X2D is a tuning mitigation pending real-device
  confirmation via the debug log, not a guaranteed fix — see above.
- This build has not been flashed to a physical H2D or X2D device by the
  maintainer; testing on real hardware is still needed to confirm all four
  fixes, especially the reconnect/watchdog change and the new `h2x2d`
  build's default Connection Mode and warning banner.
