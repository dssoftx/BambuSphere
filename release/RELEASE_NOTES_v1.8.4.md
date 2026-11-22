# BambuSphere v1.8.4 (stable)

Small feature release on top of v1.8.3. Promoted from beta to **Stable**
after testing on hardware, as the single stable build for every supported
printer. The separate localnative build is no longer offered in the web
flasher: the mainline build works in Hybrid, Cloud only and Local only mode
(choose in Web Config).

## Release Scope

- **Base**: v1.8.3 beta (Bambu Cloud login fix, camera removal, device IP,
  Default Screen, battery-% fix all included, unchanged).
- **Toolchain**: ESP-IDF v6.0.2, LVGL v9.5.0.
- **Hardware variants**: AMOLED 1.75, mainline (`all`) and localnative
  (`h2x2d` printer target) build.

## New: reset Arc Colors to defaults

Web Config > Arc Colors has a **Reset to Defaults** button. Click it, then
click again within 4 seconds to confirm. Every arc color goes back to the
built-in BambuSphere defaults and is saved immediately, no restart needed.

## Web flasher

The web flasher now offers one Stable build, v1.8.4, for all printers. The
localnative builds and the separate beta entries were removed; the Dev and
no-cloud test builds are still auto-built from `main`.
