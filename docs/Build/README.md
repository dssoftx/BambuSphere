# Building BambuSphere

This document is for developers and advanced users who want to clone, build, flash or package BambuSphere themselves. Most users should use the [BambuSphere Web Installer](https://dssoftx.github.io/BambuSphere/flash/).

## Requirements

- Windows, Linux or macOS
- Git
- Python 3
- ESP-IDF `v5.5.4`
- A USB data cable
- Enough free disk space for separate build directories

The project uses C17, C++17 and LVGL `v9.5.0`.

## Clone the repository

```powershell
git clone https://github.com/dssoftx/BambuSphere.git
cd BambuSphere
```

Activate the ESP-IDF environment before running `idf.py`. On Windows, use the ESP-IDF PowerShell or command prompt installed with ESP-IDF.

## Hardware variants

Always select the hardware explicitly. Each variant must use its own build directory.

| Variant | Build flag | Build directory | Release directory |
| --- | --- | --- | --- |
| Waveshare ESP32-S3 Touch AMOLED 1.75 | `amoled_1_75` | `build-amoled_1_75` | `release/` |
| Waveshare ESP32-S3 Touch LCD 2.8C | `lcd_2_8c` | `build-lcd_2_8c` | `release/2.8c/` |

Do not reuse one variant's build directory for the other variant.

## Printer target variant

Orthogonal to the hardware variant above, `PRINTSPHERE_PRINTER_TARGET` selects
which printer family the build is tuned for. Every variant runs the same
codebase and supports every printer model in the compatibility list — this
flag only changes a couple of defaults and a Web Config notice, not what the
firmware can connect to.

| Target | Build flag | Behavior |
| --- | --- | --- |
| Mainline (default) | `all` (or omit the flag) | Connection Mode defaults to Hybrid, as today. |
| H2 family / X2D | `h2x2d` | Connection Mode defaults to Local Only on a fresh config (Cloud/Hybrid stay selectable); Web Config shows a warning banner about known issues on this printer family (random disconnects, ESP32 running warm, Hybrid/Cloud lag without Developer Mode). |

Setting `PRINTSPHERE_PRINTER_TARGET=h2x2d` also appends `-localnative` to the
release version suffix and nests that variant's release artifacts under an
`h2x2d/` subdirectory (e.g. `release/h2x2d/` for AMOLED 1.75,
`release/2.8c/h2x2d/` for LCD 2.8C) so they never collide with the mainline
build's images.

Combined with the hardware axis, there are 4 build directories in total:
`build-amoled_1_75`, `build-amoled_1_75-h2x2d`, `build-lcd_2_8c`,
`build-lcd_2_8c-h2x2d`.

## Dev-diagnostics build

`PRINTSPHERE_DEV_DIAGNOSTICS` (`off` by default, `on` to enable) adds an
on-screen overlay to the main page for testing firmware performance: CPU
usage and free internal RAM top-left of the printer name, free PSRAM and MCU
temperature top-right (`main/src/dev_diagnostics.cpp`). It's off by default
because it enables
FreeRTOS per-task run-time stats and periodic temperature-sensor polling,
which have a small but real always-on cost that normal (including beta and
h2x2d) builds shouldn't pay.

This build is **rebuilt and republished automatically by CI** on every push
to `main` (`.github/workflows/dev-build.yml`) — the web flasher's "Dev"
entry always serves the latest `main`, so you normally don't need to build
this yourself. To build it manually anyway (AMOLED 1.75, mainline printer
target only):

```powershell
idf.py -B build-amoled_1_75-dev `
  -DPRINTSPHERE_HW_VARIANT=amoled_1_75 `
  -DPRINTSPHERE_PRINTER_TARGET=all `
  -DPRINTSPHERE_DEV_DIAGNOSTICS=on `
  -DSDKCONFIG_DEFAULTS="sdkconfig.defaults;sdkconfig.dev_diagnostics.defaults" `
  reconfigure build
```

The `SDKCONFIG_DEFAULTS` flag is required — without it, the run-time-stats
Kconfig options in `sdkconfig.dev_diagnostics.defaults` never get applied
and CPU usage will always read 0%.

`sdkconfig` (the generated, merged config) lives at the project root and is
shared across every `-B` build directory. Kconfig doesn't automatically
revert an option just because a later build didn't pass its defaults file
again, so building this variant and then building any other variant *in the
same working copy* can leave `CONFIG_FREERTOS_GENERATE_RUN_TIME_STATS=y`
enabled in a build that doesn't actually use it (harmless - the code path
stays compiled out - but wasteful). Always use a dedicated build directory
for this variant (`build-amoled_1_75-dev`, as above) and reconfigure
explicitly when switching between it and any other variant, the same as the
other three variants already require.

Package it the same way as the beta/h2x2d channels, but with `--dev`
instead of `--beta` (mirrors `--debug`/`--beta`, output goes to
`release/dev/` with no versioned archive copies - the CI workflow
overwrites this channel in place on every push rather than accumulating a
build per commit):

```powershell
python tools/package_initial_flash.py --build-dir build-amoled_1_75-dev --release-root release --dev
```

## Build the AMOLED 1.75 variant

Mainline:

```powershell
idf.py -B build-amoled_1_75 -DPRINTSPHERE_HW_VARIANT=amoled_1_75 reconfigure build
```

H2/X2D:

```powershell
idf.py -B build-amoled_1_75-h2x2d -DPRINTSPHERE_HW_VARIANT=amoled_1_75 -DPRINTSPHERE_PRINTER_TARGET=h2x2d reconfigure build
```

Build, flash and open the serial monitor (mainline shown; add
`-DPRINTSPHERE_PRINTER_TARGET=h2x2d` and use the `-h2x2d` build directory for
that variant):

```powershell
idf.py -B build-amoled_1_75 -DPRINTSPHERE_HW_VARIANT=amoled_1_75 -p COM9 build flash monitor
```

Replace `COM9` with the correct port on your system.

## Build the LCD 2.8C variant

Mainline:

```powershell
idf.py -B build-lcd_2_8c -DPRINTSPHERE_HW_VARIANT=lcd_2_8c reconfigure build
```

H2/X2D:

```powershell
idf.py -B build-lcd_2_8c-h2x2d -DPRINTSPHERE_HW_VARIANT=lcd_2_8c -DPRINTSPHERE_PRINTER_TARGET=h2x2d reconfigure build
```

Build, flash and open the serial monitor (mainline shown; add
`-DPRINTSPHERE_PRINTER_TARGET=h2x2d` and use the `-h2x2d` build directory for
that variant):

```powershell
idf.py -B build-lcd_2_8c -DPRINTSPHERE_HW_VARIANT=lcd_2_8c -p COM7 build flash monitor
```

Replace `COM7` with the correct port on your system.

## Flash or monitor without rebuilding

AMOLED 1.75:

```powershell
idf.py -B build-amoled_1_75 -DPRINTSPHERE_HW_VARIANT=amoled_1_75 -p COM9 flash
idf.py -B build-amoled_1_75 -DPRINTSPHERE_HW_VARIANT=amoled_1_75 -p COM9 monitor
```

AMOLED 1.75, H2/X2D:

```powershell
idf.py -B build-amoled_1_75-h2x2d -DPRINTSPHERE_HW_VARIANT=amoled_1_75 -DPRINTSPHERE_PRINTER_TARGET=h2x2d -p COM9 flash
idf.py -B build-amoled_1_75-h2x2d -DPRINTSPHERE_HW_VARIANT=amoled_1_75 -DPRINTSPHERE_PRINTER_TARGET=h2x2d -p COM9 monitor
```

LCD 2.8C:

```powershell
idf.py -B build-lcd_2_8c -DPRINTSPHERE_HW_VARIANT=lcd_2_8c -p COM7 flash
idf.py -B build-lcd_2_8c -DPRINTSPHERE_HW_VARIANT=lcd_2_8c -p COM7 monitor
```

LCD 2.8C, H2/X2D:

```powershell
idf.py -B build-lcd_2_8c-h2x2d -DPRINTSPHERE_HW_VARIANT=lcd_2_8c -DPRINTSPHERE_PRINTER_TARGET=h2x2d -p COM7 flash
idf.py -B build-lcd_2_8c-h2x2d -DPRINTSPHERE_HW_VARIANT=lcd_2_8c -DPRINTSPHERE_PRINTER_TARGET=h2x2d -p COM7 monitor
```

## Package both release variants

Build both mainline hardware variants first. Then run:

```powershell
powershell -ExecutionPolicy Bypass -File tools/package_release.ps1 -Version v1.7.0
```

The script creates the four current release images plus versioned archive copies:

| Hardware | Initial USB image | OTA image |
| --- | --- | --- |
| AMOLED 1.75 | `release/initial/printsphere_full.bin` | `release/ota/printsphere_ota.bin` |
| LCD 2.8C | `release/2.8c/initial/printsphere_full.bin` | `release/2.8c/ota/printsphere_ota.bin` |

Versioned copies are stored below the corresponding `archive/` directories.

`tools/package_release.ps1` only knows about the two mainline hardware
variants (`PRINTSPHERE_PRINTER_TARGET=all`). Package `h2x2d` builds manually,
below.

## Package one variant manually

AMOLED 1.75:

```powershell
python tools/package_initial_flash.py --build-dir build-amoled_1_75 --release-root release --version v1.7.0
```

LCD 2.8C:

```powershell
python tools/package_initial_flash.py --build-dir build-lcd_2_8c --release-root release/2.8c --version v1.7.0-2.8c
```

AMOLED 1.75, H2/X2D:

```powershell
python tools/package_initial_flash.py --build-dir build-amoled_1_75-h2x2d --release-root release/h2x2d --version v1.7.0-localnative
```

LCD 2.8C, H2/X2D:

```powershell
python tools/package_initial_flash.py --build-dir build-lcd_2_8c-h2x2d --release-root release/2.8c/h2x2d --version v1.7.0-2.8c-localnative
```

## Initial image versus OTA image

- `printsphere_full.bin` is a merged image containing the bootloader, partition table, OTA data and application. Use it for an empty device, recovery or a partition-layout migration.
- `printsphere_ota.bin` contains only the application. Install it through the running device's Web Config so ESP-IDF writes it to the inactive OTA slot and preserves NVS configuration.

Do not use an OTA image as a full image at address `0x0`.

Devices on v1.5.x or older require one full v1.6.x flash because v1.6 changed the partition layout. Devices already using the v1.6 layout can normally use OTA.

## Manual full-image flashing

AMOLED 1.75:

```powershell
esptool.exe --chip esp32s3 --port COM9 write_flash 0x0 release/initial/printsphere_full.bin
```

LCD 2.8C:

```powershell
esptool.exe --chip esp32s3 --port COM7 write_flash 0x0 release/2.8c/initial/printsphere_full.bin
```

The merged image already contains the bootloader and partition table.

## Reconfigure and clean builds

Use `reconfigure` first when CMake settings, dependencies or hardware/printer-target options change:

```powershell
idf.py -B build-amoled_1_75 -DPRINTSPHERE_HW_VARIANT=amoled_1_75 reconfigure
idf.py -B build-lcd_2_8c -DPRINTSPHERE_HW_VARIANT=lcd_2_8c reconfigure
idf.py -B build-amoled_1_75-h2x2d -DPRINTSPHERE_HW_VARIANT=amoled_1_75 -DPRINTSPHERE_PRINTER_TARGET=h2x2d reconfigure
idf.py -B build-lcd_2_8c-h2x2d -DPRINTSPHERE_HW_VARIANT=lcd_2_8c -DPRINTSPHERE_PRINTER_TARGET=h2x2d reconfigure
```

`fullclean` is normally unnecessary. If a build cache is genuinely broken, remove or recreate only the affected variant's build directory.

## Important project files

| Path | Purpose |
| --- | --- |
| `CMakeLists.txt` | Release version, hardware selection and packaging target |
| `main/CMakeLists.txt` | Application sources and ESP-IDF component dependencies |
| `sdkconfig.defaults` | Shared ESP-IDF project defaults |
| `partitions.csv` | NVS, OTA and LittleFS partition layout |
| `tools/package_release.ps1` | Packages both hardware variants |
| `tools/package_initial_flash.py` | Creates merged initial and OTA images |
| `flash/index.html` | Browser installer page |
| `release/` | Published firmware images and release notes |

## Release checklist

1. Set the release version in the root `CMakeLists.txt`.
2. Build and test both hardware variants.
3. Package both variants with an explicit version.
4. Verify the four generated images and their versioned archive copies.
5. Update the browser installer and release notes.
6. Commit and publish the source, installer, notes and binaries together.
