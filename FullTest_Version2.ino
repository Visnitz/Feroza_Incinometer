/**
  FullTest.ino
  Quick hardware test sketch for JXL ESP32S3 dev board based on FullFunctionTest.ino
  Tests: Serial info, I2C scan, Touch basic read, SD init, Display init + fill,
         Battery ADC read, WS2812 LED blink.

  Usage:
   - Place this file in the project root (Arduino IDE) or in src/ (PlatformIO)
   - Ensure src/config.h exists (as generated)
*/

#include <Arduino.h>
#include "config.h"

#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Arduino_GFX_Library.h> // Arduino_GFX + GC9A01
#include <Adafruit_NeoPixel.h>

Adafruit_NeoPixel strip(WS2812_COUNT, WS2812_PIN, NEO_GRB + NEO_KHZ800);

// Create display bus and gfx objects (matches FullFunctionTest layout)
Arduino_DataBus *bus = nullptr;
Arduino_GFX *gfx = nullptr;

void printHeader() {
  Serial.println("=== JXL ESP32S3 Quick Hardware Test ===");
  uint32_t flash_size = 0;
  esp_flash_get_size(NULL, &flash_size);
  Serial.printf("Flash Size: %u bytes (%.2f MB)\r\n", flash_size, flash_size / (1024.0 * 1024.0));
  size_t psram_size = heap_caps_get_total_size(MALLOC_CAP_SPIRAM);
  if (psram_size > 0) {
    Serial.printf("PSRAM Size: %u bytes (%.2f MB)\r\n", (unsigned)psram_size, psram_size / (1024.0 * 1024.0));
  } else {
    Serial.println("No PSRAM detected");
  }
}

// Simple I2C scanner
void testI2C() {
  Serial.println("\n[I2C] Scanning bus...");
  I2C_BEGIN();
  bool found = false;
  for (uint8_t addr = 1; addr < 127; ++addr) {
    Wire.beginTransmission(addr);
    uint8_t err = Wire.endTransmission();
    if (err == 0) {
      Serial.printf(" I2C device found at 0x%02X\n", addr);
      found = true;
    }
  }
  if (!found) Serial.println(" No I2C devices found.");
  delay(200);
}

// Touch basic init and ID read (reads register 0xA7)
void testTouch() {
  Serial.println("\n[Touch] Reset and probe...");
  pinMode(TOUCH_RST_PIN, OUTPUT);
  digitalWrite(TOUCH_RST_PIN, LOW);
  delay(10);
  digitalWrite(TOUCH_RST_PIN, HIGH);
  delay(50);

  Wire.beginTransmission(TOUCH_I2C_ADDR);
  Wire.write(0xA7); // ChipId register (as in demo)
  if (Wire.endTransmission(false) != 0) {
    Serial.println(" Touch device not responding (endTransmission).");
    return;
  }
  Wire.requestFrom(TOUCH_I2C_ADDR, 1, true);
  if (Wire.available()) {
    uint8_t id = Wire.read();
    Serial.printf(" Touch chip ID: 0x%02X\n", id);
  } else {
    Serial.println(" Touch: no response to ID read");
  }
}

// SD card init
void testSD() {
  Serial.println("\n[SD] Initializing SD card...");
  // Ensure SPI pins are set according to config
  SPI.begin(SD_SCK_PIN, SD_MISO_PIN, SD_MOSI_PIN, SD_CS_PIN);
  delay(10);
  if (SD.begin(SD_CS_PIN, SPI, 80000000)) {
    Serial.println(" SD initialized successfully.");
    if (SD.exists("/")) Serial.println(" Root directory accessible.");
  } else {
    Serial.println(" SD mount failed.");
  }
}

// Display init + simple fill
void testDisplay() {
  Serial.println("\n[Display] Initializing display...");
  // instantiate bus/gfx using config macros
  if (!bus) {
    bus = new Arduino_ESP32LCD8(DISP_DC, DISP_CS, DISP_WR, DISP_RD,
                                DISP_D0, DISP_D1, DISP_D2, DISP_D3,
                                DISP_D4, DISP_D5, DISP_D6, DISP_D7);
  }
  if (!gfx) {
    gfx = new Arduino_GC9A01(bus, DISP_RST, 0 /*rotation*/, true /*IPS*/);
  }

  gfx->begin();
  // backlight PWM
  pinMode(DISP_BL, OUTPUT);
  ledcSetup(1, 5000, 8);
  ledcAttachPin(DISP_BL, 1);
  ledcWrite(1, 200); // ~80% brightness
  delay(50);

  gfx->fillScreen(BLACK);
  gfx->setTextColor(WHITE, BLACK);
  gfx->setTextSize(2);
  gfx->setCursor(10, 10);
  gfx->print("Display OK");
  gfx->setCursor(10, 40);
  gfx->print("240x240 GC9A01");
  Serial.println(" Display: filled and text printed.");
}

// Battery ADC read
void testBattery() {
  Serial.println("\n[Battery] Reading ADC...");
  // Ensure the pin is ADC capable on your board
  int raw = analogRead(BAT_PIN);
  float voltage = (raw / 4095.0f) * 3.3f * 2.0f; // according to ADC_Magnification=2
  Serial.printf(" Raw ADC: %d, Voltage ~ %.2f V\n", raw, voltage);
}

// WS2812 test
void testWS2812() {
  Serial.println("\n[WS2812] Testing NeoPixel...");
  strip.begin();
  strip.show();
  strip.setPixelColor(0, strip.Color(20, 0, 0));
  strip.setPixelColor(1, strip.Color(0, 20, 0));
  strip.show();
  delay(500);
  strip.setPixelColor(0, strip.Color(0, 20, 0));
  strip.setPixelColor(1, strip.Color(20, 0, 0));
  strip.show();
  delay(500);
  strip.clear();
  strip.show();
  Serial.println(" WS2812: blink sequence done.");
}

void setup() {
  Serial.begin(115200);
  delay(100);
  printHeader();

  testI2C();
  testTouch();
  testSD();
  testDisplay();
  testBattery();
  testWS2812();

  Serial.println("\nAll tests executed once. Re-run individual tests from Serial monitor by resetting board.");
}

void loop() {
  // Idle: update a tiny status on display every 3s to show alive
  static uint32_t last = 0;
  if (millis() - last > 3000) {
    last = millis();
    if (gfx) {
      gfx->setCursor(10, 200);
      gfx->setTextColor(WHITE, BLACK);
      gfx->setTextSize(1);
      gfx->printf("Uptime: %lu s  ", millis() / 1000);
    }
    // print a lightweight serial heartbeat
    Serial.printf("Heartbeat: %lu s\n", millis() / 1000);
  }
  delay(10);
}