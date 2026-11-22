# BambuSphere v1.8.4 (beta)

Small feature release on top of v1.8.3. Intended to become the next stable
release once confirmed on hardware.

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

The localnative builds are no longer labelled "H2/X2D" in the web flasher.
They are the local-first builds (Connection Mode defaults to Local Only) and
apply to any printer, not just the H2 family and X2D.
