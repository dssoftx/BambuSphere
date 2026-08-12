# BambuSphere v1.8.2 (beta)

Fix release on top of v1.8.1, focused on the multi-printer switching path and
the main-page layout. Published as a beta while these changes get more
real-world testing; supersedes the v1.8.1 beta, which is not otherwise
changed.

## Release Scope

- **Base**: v1.8.1 (itself unreleased as Stable — still carries the more
  Bambu Cloud regions, 12h/24h clock toggle, and optional layer-count line
  from that release).
- **Toolchain target**: ESP-IDF v5.5.4, LVGL v9.5.0.
- **Hardware variants**: AMOLED 1.75, mainline (`all`) and `h2x2d`
  printer-target build. LCD 2.8C is no longer part of this project (removed
  after v1.6.2).

## Fixes

### Stale progress from the previous printer right after switching

Switching printers (vertical swipe or the web setup portal) sometimes kept
showing the *previous* printer's progress/status briefly afterward,
especially noticeable while a printer was "Preparing." Cause: the Bambu
Cloud connection is a single account-level session shared across every
configured printer, and which printer it's currently scoped to is switched
on its own background task rather than instantly. The main display loop had
no way to tell that a cloud snapshot it just read still belonged to the
printer you'd just swiped away from, so it could briefly merge that stale
data into the newly active printer's display. The display now recognizes
when the cloud connection hasn't caught up yet to the printer you just
switched to and skips that data for those few ticks, instead of showing the
wrong printer.

### Battery indicator overlapping the top percentage

On the main page, the big percentage readout is enlarged well past its
normal size, and its glyphs reached down far enough to overlap the battery
icon/percentage shown just below it whenever the device was running on
battery. The battery indicator now sits lower on the main page, between the
big percentage and the printer status line, clear of both. Its position on
the camera page (where it doubles as the print-status readout) is
unchanged.

## Known Notes

- One report of Bambu Cloud 2FA being rejected for an Australian user is
  still open — investigated (full login/2FA code path, no region-specific
  logic found) but not fixed pending more detail (e.g. a device debug log)
  to confirm root cause.
- Not yet flashed to a physical device by the maintainer for this specific
  build; testing is still needed to confirm both fixes on real hardware,
  particularly the printer-switch timing fix, which depends on real MQTT
  network conditions.
