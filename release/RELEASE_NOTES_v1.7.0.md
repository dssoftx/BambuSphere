# PrintSphere v1.7.0

Feature release on top of v1.6.2 focused on a reworked touch UI: a new clock
page, a dedicated on-device brightness page, vertical-swipe printer switching,
and a redesigned main dashboard. OTA-compatible with v1.6.x (no partition
table change).

## Release Scope

- **Base**: v1.6.2. Devices on v1.6.x can use the OTA update path.
- **Toolchain target**: ESP-IDF v5.5.4, LVGL v9.5.0.
- **Hardware variants**: Waveshare ESP32-S3 Touch AMOLED 1.75 and ESP32-S3
  Touch LCD 2.8C.

## Major Changes

- **New clock page**: replaces the cloud cover-image page with a large
  current-time display and a remaining-time chip. Always available,
  regardless of connection mode, since it no longer depends on a cloud
  cover image.
- **New self-settings page**: a dedicated page (swipe all the way right)
  with a vertical brightness control. Drag up/down on this page to adjust
  display brightness; the chosen level is now persisted across reboots.
- **Vertical swipe switches printers**: on any page other than the printer
  list and the new settings page, swiping up/down cycles to the
  next/previous configured printer (a no-op with only one printer
  configured). The printer list page keeps its existing tap-to-switch
  cards as a second way to switch.
- **Redesigned main dashboard**: progress percentage, lifecycle status,
  printer name (or Wi-Fi setup info when no printer is connected), nozzle
  and bed temperature chips, and the remaining-time chip have all been
  repositioned and resized for better legibility on the round display.
- **Bambu Lab logo removed from the main page**: the center of the main
  page is now used for the printer name. This also removes the
  tap-to-toggle chamber-light shortcut that badge doubled as; there is no
  replacement control for chamber light on the main page in this release.

## Internal Changes

- Removed the PNG cover-image decode/display path (`decode_preview_png`
  and related preview-image plumbing in the UI layer) along with the
  `libpng`-based rendering it drove for the retired preview page.
- Added a persisted display-brightness setting in NVS
  (`ConfigStore::load/save_display_brightness_percent`).
- Fixed a data race on the printer-card cache introduced while adding the
  vertical-swipe printer switch: `Ui::update_printer_cards()` now updates
  the cache under the same lock the new swipe-cycle code reads it with.

## Known Notes

- Chamber-light toggling from the touchscreen is no longer available on
  the main page (it was previously reachable by tapping the Bambu Lab
  logo badge, which has been removed).
- The LCD 2.8C hardware variant was not rebuilt or repackaged for this
  release; its release images under `release/2.8c/` remain at v1.6.2.
