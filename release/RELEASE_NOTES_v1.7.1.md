# BambuSphere v1.7.1 (beta)

Bugfix release on top of v1.7.0. Published as a beta/unstable build while
these fixes get more real-world testing on affected hardware.

## Release Scope

- **Base**: v1.7.0.
- **Toolchain target**: ESP-IDF v5.5.4, LVGL v9.5.0.
- **Hardware variants**: Waveshare ESP32-S3 Touch AMOLED 1.75 only. The
  LCD 2.8C variant was not rebuilt for this release.

## Fixes

- **Main-page printer name hidden behind an HMS code on some models**: on
  printers where the current HMS/error code has no entry in the embedded
  lookup table (observed on the H2D), the main page was permanently
  showing a raw, unresolved diagnostic string (formatted like
  `HMS XXXXXXXX_XXXXXXXX`) instead of the printer's configured name — the
  printer-select page already showed the correct name, so this was
  specific to the main dashboard's fallback logic. The main page now
  falls back to the printer name whenever detail_text() has nothing more
  useful than that raw, unresolved code; real resolved HMS/error messages
  and job names still take priority as before.
- **Dual-nozzle temperature text wrapping**: on printers that report a
  left/right active nozzle (e.g. H2D), the nozzle temperature chip
  prefixes the reading with "L "/"R ", which no longer fits the dosis_40
  font inside its fixed-width box and wrapped to a second line, breaking
  the chip's layout. The nozzle and bed temperature values now use
  dosis_32 (one size down) with clip-instead-of-wrap as a safety net, so
  the extra prefix character fits without ever splitting the value across
  two lines.

## Known Notes

- This build has not been flashed to a physical H2D device by the
  maintainer; testing is still needed to confirm both fixes render
  correctly on that hardware.
