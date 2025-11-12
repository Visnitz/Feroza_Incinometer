```markdown
# JXL ESP32S3 Quick Hardware Test

Ez a rövid teszt projekt gyorsan ellenőrzi a legfontosabb perifériákat a JXL ESP32S3 dev boardon.

Tartalom:
- src/config.h  — pin map (auto-generated)
- FullTest.ino  — a teszt sketch
- platformio.ini — PlatformIO konfiguráció és lib_deps

Könyvtárak (PlatformIO lib_deps):
- Arduino_GFX_Library (1.4.7)
- Adafruit NeoPixel (1.12.5)
- Wire/SPI/SD: framework‑be épített

Ha Arduino IDE-t használsz:
- Library Managerből telepítsd: "Arduino_GFX_Library" és "Adafruit NeoPixel".
- Wire, SPI, SD könyvtárak az alap Arduino core részei.

Használat:
1. Másold be a `src/config.h` fájlt és a `FullTest.ino` sketch-et a projektedbe.
2. PlatformIO: pio run -t upload -e esp32-s3-dev
   Arduino IDE: válaszd ki a megfelelő ESP32S3 boardot és töltsd fel.
3. Nyisd meg a soros monitort 115200 baudon és figyeld a kimenetet.
4. Végezd el az I2C-scannert kívánt I2C eszközök detektálásához.
5. Ellenőrizd a kijelzőn a "Display OK" üzenetet és a sárga/kék WS2812 villanást.
6. Ha boot/programozási problémát észlelsz, ellenőrizd a sémát: GPIO0/GPIO1/GPIO3 használata boot vagy UART funkciókra ütközhet.

Megjegyzések:
- TOUCH_RST_PIN (GPIO0) használata potenciálisan befolyásolhatja a boot módot; ha problémát tapasztalsz, javasolt ezt más, nem boot-strap pinre átmozgatni.
- BAT_PIN (GPIO1) gyakran UART0 TX; ha soros kimenet nincs, érdemes alternatív ADC‑t használni.
- Ha a QMI8658 IMU-hoz van speciális könyvtárad, illeszd be a lib_deps-be és add meg a megfelelő inicializációt.

```