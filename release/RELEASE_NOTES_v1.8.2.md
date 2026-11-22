# BambuSphere v1.8.2 (beta)

Feature/cleanup release on top of the v1.8.1 beta. Published as a beta while
these changes get real-world testing.

## Release Scope

- **Base**: v1.8.1 beta (`c4104a4`). The fixes that landed on `main` after
  v1.8.1 (`d3f90e3`, `ad59ef9`) are **not** included — they were reverted
  as broken.
- **Toolchain**: ESP-IDF v6.0.2, LVGL v9.5.0.
- **Hardware variants**: AMOLED 1.75, mainline (`all`) and `h2x2d`
  (localnative) printer-target build.

## Removed: camera page

The camera page and the local JPEG camera client were removed entirely. A
Bambu Lab API change broke the camera, so it only ever showed an error. The
pager now goes straight from the clock page to the brightness page. Also
removed: the Hybrid-mode logic that existed only for the camera (camera
demand / cloud cooldown after leaving the camera page) and the camera
mentions in Web Config.

## New: device IP on the last page

The brightness page (last tab) now shows the device's IP address below the
brightness percentage, so you can find Web Config without looking up the
router. Before Wi-Fi connects it shows the setup access point address
instead, or "No Wi-Fi".

## New: Default Screen

Web Config > Display Preferences has a new **Default Screen** option that
picks the tab the device opens on at power-up: Main dashboard (default),
Printer list, AMS, Clock or Brightness / info. The AMS tab only appears once
the printer reports its AMS, so that choice starts on the main page and
jumps over as soon as the data arrives. It falls back to Main if there is no
AMS, if you touch the screen first, or if 30 s pass.

## Fixed: battery % overlapping the print percentage

On the main page the battery percentage overlapped the enlarged print
percentage. It now sits further right; the battery icon and everything else
stay where they were.

## Known Notes

- Not yet flashed to a physical device by the maintainer for this specific
  build; please confirm the battery-% position and Default Screen behaviour
  on real hardware.
- Default Screen applies at boot. Plugging USB into an already running,
  battery-powered unit doesn't switch tabs.
