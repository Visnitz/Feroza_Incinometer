#pragma once
// Auto-generated pin map from FullFunctionTest.ino (JXL ESP32S3 dev board)
// Use: #include "config.h" in your Arduino sketch

// ---- Display (GC9A01, 8-bit parallel) ----
#define DISP_DC     18
#define DISP_CS     2
#define DISP_WR     3
#define DISP_RD     -1   // not used
#define DISP_D0     10
#define DISP_D1     11
#define DISP_D2     12
#define DISP_D3     13
#define DISP_D4     14
#define DISP_D5     15
#define DISP_D6     16
#define DISP_D7     17
#define DISP_RST    21
#define DISP_BL     42   // backlight PWM

// ---- I2C bus ----
#define I2C_SDA     8
#define I2C_SCL     9

// I2C device addresses
#define TCA6408_I2C_ADDR   0x20
#define TOUCH_I2C_ADDR     0x15
#define PCF85063_I2C_ADDR  0x51
// QMI8658 (IMU) uses the same I2C bus (check its driver for address)

// TCA6408 interrupt pin (input, FALLING)
#define TCA6408_INT_PIN    45

// Touch
#define TOUCH_RST_PIN      0  // caution: GPIO0 can affect boot mode

// ---- SD card (SPI) ----
#define SD_SCK_PIN    41
#define SD_MISO_PIN   48
#define SD_MOSI_PIN   47
#define SD_CS_PIN     40

// ---- WS2812 ----
#define WS2812_PIN    46
#define WS2812_COUNT  2

// ---- Battery ADC ----
#define BAT_PIN       1   // analogRead(BAT_PIN) ; caution: GPIO1 is often UART0 TX

// ---- Misc ----
// MJPEG buffer size (example)
#define MJPEG_BUFFER_SIZE (240 * 240 * 60)

// Helpful macros
#define I2C_BEGIN() Wire.begin(I2C_SDA, I2C_SCL); Wire.setClock(400000);

// Notes:
// - Some pins (GPIO0, GPIO1, GPIO3, and others) may be used for UART0, boot-strapping, or flash/PSRAM signals on this dev board.
//   Check the board schematic before reassigning these pins to peripherals.
// - If you experience boot/programming issues, try moving functionality off GPIO0/GPIO1/GPIO3.
// - Example use in sketch:
//     #include "config.h"
//     void setup() {
//       Serial.begin(115200);
//       I2C_BEGIN();
//       pinMode(DISP_BL, OUTPUT);
//       ledcSetup(1, 5000, 8); ledcAttachPin(DISP_BL, 1);
//       // ...
//     }