# BambuSphere

BambuSphere is a round ESP32-S3 companion display for Bambu Lab printers. It shows print progress, temperatures, remaining time, AMS information, errors and supported camera snapshots.

It works directly with Bambu Cloud, the printer's local connection, or both. Home Assistant is not required.

<img width="400" height="300" alt="BambuSphere AMOLED display" src="https://github.com/user-attachments/assets/820c2e9b-10a7-4430-949c-e8b0adc1357d" /> <img width="400" height="300" alt="BambuSphere interface" src="https://github.com/user-attachments/assets/5923dc59-0123-4df1-b54d-673c6dbad23b" />

Latest stable version: **v1.7.0**

## About this fork

BambuSphere is a fork of [PrintSphere](https://github.com/cptkirki/PrintSphere) by
Cpt_Kirk, reworked with a new touch UI: a dedicated clock page, an on-device
display-brightness page, vertical-swipe printer switching, and a redesigned
main dashboard. See the [v1.7.0 release notes](release/RELEASE_NOTES_v1.7.0.md)
for the full list of changes. Licensed under the Federation Non-Commercial
License (FNCL) v1.1 — see [LICENSE](LICENSE); this project is non-commercial
only.

## Supported hardware

Choose the correct hardware in the installer:

| Hardware | Installer name | Battery support |
| --- | --- | --- |
| Waveshare ESP32-S3 Touch AMOLED 1.75 | `1.75 AMOLED` | Yes, including charging and USB detection |
| Waveshare ESP32-S3 Touch LCD 2.8C | `2.8" LCD` | Yes, using the board's battery measurement |

Firmware for one variant must not be installed on the other variant.

## Install BambuSphere

You need a USB data cable and a desktop version of Chrome or Edge.

1. Open the **[BambuSphere Web Installer](https://dssoftx.github.io/BambuSphere/flash/)**.
2. Select your hardware variant.
3. Connect the device by USB and choose **Install selected firmware**.
4. Keep the USB cable connected after installation.
5. The installer searches for nearby Wi-Fi networks and sends your Wi-Fi details directly to BambuSphere over USB.
6. Open Web Config using the IP address shown by BambuSphere and connect your Bambu printer.

### Wi-Fi fallback

If USB Wi-Fi setup is unavailable, connect to the fallback access point:

- Wi-Fi name: `BambuSphere-Setup`
- Password: `bambusphere`
- Setup page: [http://192.168.4.1](http://192.168.4.1)

Select your home Wi-Fi there. After BambuSphere connects, continue through Web Config on its home-network IP address.

## Connect your printer

Web Config offers three connection modes:

- **Hybrid (recommended):** combines cloud and local data and automatically uses the better status path. Local camera snapshots remain available on supported printers.
- **Cloud only:** uses Bambu Cloud for status. Local MQTT and local camera snapshots are disabled.
- **Local only:** connects directly to the printer without requiring a Bambu Cloud account.

For Bambu Cloud, enter your account details and complete an email-code or 2FA step if requested.

For a local connection, enter:

- Printer IP address or hostname
- Printer serial number
- Printer access code

Cloud and local printer settings can normally be connected without restarting BambuSphere.

## Update without losing your settings

Use an **OTA update** for an already configured BambuSphere. OTA keeps Wi-Fi, printer profiles and display settings.

1. Open the [BambuSphere Web Installer](https://dssoftx.github.io/BambuSphere/flash/).
2. Select the same hardware variant currently installed on your device.
3. In **OTA Update**, enter the IP address shown by BambuSphere.
4. Open the OTA updater and confirm **Flash from URL** in Web Config.

You can also open Web Config directly and use its **Firmware Update** section. It accepts either a matching OTA `.bin` file or a GitHub firmware URL.

The public installer performs the initial USB flash. OTA itself runs through Web Config so the firmware can safely update the inactive application slot and retain the configuration.

> Devices running v1.5.x or older need one initial USB flash before using v1.6.x+ OTA images because v1.6 introduced a new partition layout.

## Main features

- Print progress, remaining time and temperatures
- A dedicated clock page showing the current time and remaining print time
- AMS and external-spool information
- Local JPEG camera snapshots on supported printers
- Pause, resume and stop controls
- Chamber-light control on supported printers
- Multi-printer profiles with a vertical swipe gesture to switch between them, or live printer switching from the printer list
- Bambu HMS and error descriptions on the device
- On-device display brightness control
- Adjustable display rotation and status colors
- Battery-aware dimming and screen-off settings
- Configurable sound notifications and custom WAV files
- Secure Web Config access using a temporary PIN
- OTA firmware updates that retain the device configuration

## Printer and camera support

BambuSphere contains status paths for:

`A1`, `A1 Mini`, `A2L`, `P1P`, `P1S`, `P2S`, `H2C`, `H2D`, `H2D Pro`, `H2S`, `X1`, `X1C`, `X1E`, `X2D`

Local JPEG snapshots are available on `A1`, `A1 Mini`, `A2L`, `P1P` and `P1S`.

`P2S`, the `H2` family, the `X1` family and `X2D` use RTSP video. The ESP32-S3 cannot decode these streams, so their live camera view is not available in BambuSphere.

The `H2` family and `X2D` require Developer Mode for local printer status. Availability of individual values can differ between printer models and firmware versions.

## Web Config

Web Config includes:

- Wi-Fi scanning and configuration
- Cloud and local printer connections
- Connection mode and printer selection
- Display rotation, colors and time zone
- Battery, USB and screen power-saving options
- AMS display behavior
- Sound notification settings
- Portal PIN protection
- Firmware updates by file upload or URL

After initial setup, Web Config can be protected with a temporary six-digit PIN. Long-press the BambuSphere display for about one second to request a PIN.

## Known limitations

- RTSP camera streams cannot be displayed on the ESP32-S3.
- Newer printer families have received less real-world testing than `P1S` and `P1P`.
- Some local V2 protocol values can differ from the older V1 printer fields.
- Cloud features require internet access and a working Bambu Cloud account.
- Chamber-light toggling from the touchscreen is not available on the main page in v1.7.0 (see the release notes).

## Links

- [BambuSphere Web Installer](https://dssoftx.github.io/BambuSphere/flash/)
- [v1.7.0 release notes](release/RELEASE_NOTES_v1.7.0.md)
- [v1.7.1 (beta) release notes](release/RELEASE_NOTES_v1.7.1.md) — dual-nozzle and HMS-code display fixes, available in the installer's beta channel
- [Building, cloning and manual flashing](docs/Build/README.md)
- [Original PrintSphere project](https://github.com/cptkirki/PrintSphere)
- [License](LICENSE)
