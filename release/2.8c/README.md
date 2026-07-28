# BambuSphere 2.8C Release

Diese Dateien sind die vorkonfektionierten Firmware-Artefakte fuer das Board
`ESP32-S3-Touch-LCD-2.8C`.

Ordner:

- `initial/` = Komplett-Image fuer den ersten Flash
- `ota/` = App-only Image fuer OTA-Updates
- `beta/` = optionale Beta-/Debug-Pakete fuer das 2.8C-Board

Packaging-Beispiel aus dem Projektroot:

```powershell
python tools/package_initial_flash.py --build-dir build-lcd_2_8c --release-root release/2.8c --version v1.6.1-2.8c
```
