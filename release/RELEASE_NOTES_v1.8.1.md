# BambuSphere v1.8.1 (beta)

Feature/fix release on top of v1.8.0, focused on the web configurator and
the main-page clock. Published as a beta while these changes get more
real-world testing.

## Release Scope

- **Base**: v1.8.0.
- **Toolchain target**: ESP-IDF v5.5.4, LVGL v9.5.0.
- **Hardware variants**: AMOLED 1.75, mainline (`all`) and `h2x2d`
  printer-target build. LCD 2.8C is no longer part of this project (removed
  after v1.6.2).

## New: more Bambu Cloud regions

The Cloud Region picker in the web configurator only listed US/EU/CN. All
non-China regions already route through the same global endpoint
(`api.bambulab.com`), so this was a labeling gap, not a functional one —
users outside those three regions (e.g. Australia/Oceania) had no accurate
option to select. Added South America, Africa, Asia and Oceania/Australia
alongside the existing three.

## New: 12-hour clock option

The web configurator's new "Display Preferences" panel adds a clock format
toggle (24-hour / 12-hour), applied to both the ETA countdown row and the
big Page-2 clock. The 12-hour format originally appended an "AM"/"PM"
suffix, which overflowed the clock widget whenever the hour was two digits
(10, 11, 12) — the suffix has been removed; 12-hour mode now just shows
`H:MM`.

## New: optional layer-count line

"Display Preferences" also adds a "Show Layer Lines" toggle, which shows a
`Layer: X / Y` line under the printer name on the main page. This reuses a
row that already existed in the layout but was previously always hidden.
Off by default, so existing users see no change unless they opt in.

## Known Notes

- One report of Bambu Cloud 2FA being rejected for an Australian user is
  still open — investigated (full login/2FA code path, no region-specific
  logic found) but not fixed pending more detail (e.g. a device debug log)
  to confirm root cause.
- Not yet flashed to a physical device by the maintainer for this specific
  build; testing is still needed to confirm the clock/layer-lines/region
  changes on real hardware.
