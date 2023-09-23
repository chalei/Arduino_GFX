/*******************************************************************************
 * Start of Arduino_GFX setting
 *
 * Arduino_GFX try to find the settings depends on selected board in Arduino IDE
 * Or you can define the display dev kit not in the board list
 * Defalult pin list for non display dev kit:
 * Arduino Nano, Micro and more: CS:  9, DC:  8, RST:  7, BL:  6, SCK: 13, MOSI: 11, MISO: 12
 * ESP32 various dev board     : CS:  5, DC: 27, RST: 33, BL: 22, SCK: 18, MOSI: 23, MISO: nil
 * ESP32-C3 various dev board  : CS:  7, DC:  2, RST:  1, BL:  3, SCK:  4, MOSI:  6, MISO: nil
 * ESP32-S2 various dev board  : CS: 34, DC: 38, RST: 33, BL: 21, SCK: 36, MOSI: 35, MISO: nil
 * ESP32-S3 various dev board  : CS: 40, DC: 41, RST: 42, BL: 48, SCK: 36, MOSI: 35, MISO: nil
 * ESP8266 various dev board   : CS: 15, DC:  4, RST:  2, BL:  5, SCK: 14, MOSI: 13, MISO: 12
 * Raspberry Pi Pico dev board : CS: 17, DC: 27, RST: 26, BL: 28, SCK: 18, MOSI: 19, MISO: 16
 * RTL8720 BW16 old patch core : CS: 18, DC: 17, RST:  2, BL: 23, SCK: 19, MOSI: 21, MISO: 20
 * RTL8720_BW16 Official core  : CS:  9, DC:  8, RST:  6, BL:  3, SCK: 10, MOSI: 12, MISO: 11
 * RTL8722 dev board           : CS: 18, DC: 17, RST: 22, BL: 23, SCK: 13, MOSI: 11, MISO: 12
 * RTL8722_mini dev board      : CS: 12, DC: 14, RST: 15, BL: 13, SCK: 11, MOSI:  9, MISO: 10
 * Seeeduino XIAO dev board    : CS:  3, DC:  2, RST:  1, BL:  0, SCK:  8, MOSI: 10, MISO:  9
 * Teensy 4.1 dev board        : CS: 39, DC: 41, RST: 40, BL: 22, SCK: 13, MOSI: 11, MISO: 12
 ******************************************************************************/
#include <Arduino_GFX_Library.h>
#include <PCA95x5.h>
#include <PacketSerial.h>

PacketSerial myPacketSerial;

#define RXD2 20
#define TXD2 19

#define PKT_TYPE_SENSOR_SCD41_CO2 0XB2
#define PKT_TYPE_SENSOR_SHT41_TEMP 0XB3
#define PKT_TYPE_SENSOR_SHT41_HUMIDITY 0XB4
#define PKT_TYPE_SENSOR_TVOC_INDEX 0XB5
#define GFX_BL DF_GFX_BL // default backlight pin, you may replace DF_GFX_BL to actual backlight pin

/* More dev device declaration: https://github.com/moononournation/Arduino_GFX/wiki/Dev-Device-Declaration */
#if defined(DISPLAY_DEV_KIT)
Arduino_GFX *gfx = create_default_Arduino_GFX();
#else /* !defined(DISPLAY_DEV_KIT) */

#define GFX_DEV_DEVICE ESP32_S3_RGB
#define GFX_BL 45
Arduino_DataBus *bus = new Arduino_SWSPI(
    GFX_NOT_DEFINED /* DC */, PCA95x5::Port::P04 /* CS */,
    41 /* SCK */, 48 /* MOSI */, GFX_NOT_DEFINED /* MISO */);

// option 1:
// Uncomment for 4" rect display
Arduino_ESP32RGBPanel *rgbpanel = new Arduino_ESP32RGBPanel(
    18 /* DE */, 17 /* VSYNC */, 16 /* HSYNC */, 21 /* PCLK */,
    4 /* R0 */, 3 /* R1 */, 2 /* R2 */, 1 /* R3 */, 0 /* R4 */,
    10 /* G0 */, 9 /* G1 */, 8 /* G2 */, 7 /* G3 */, 6 /* G4 */, 5 /* G5 */,
    15 /* B0 */, 14 /* B1 */, 13 /* B2 */, 12 /* B3 */, 11 /* B4 */,
    1 /* hsync_polarity */, 10 /* hsync_front_porch */, 8 /* hsync_pulse_width */, 50 /* hsync_back_porch */,
    1 /* vsync_polarity */, 10 /* vsync_front_porch */, 8 /* vsync_pulse_width */, 20 /* vsync_back_porch */);
Arduino_RGB_Display *gfx = new Arduino_RGB_Display(
    480 /* width */, 480 /* height */, rgbpanel, 2 /* rotation */, true /* auto_flush */,
    bus, GFX_NOT_DEFINED /* RST */, st7701_type1_init_operations, sizeof(st7701_type1_init_operations));

#endif /* !defined(DISPLAY_DEV_KIT) */
/*******************************************************************************
 * End of Arduino_GFX setting
 ******************************************************************************/

/* more fonts at: https://github.com/moononournation/ArduinoFreeFontFile.git */
#include "FreeMono8pt7b.h"
#include "FreeSansBold10pt7b.h"
#include "FreeSerifBoldItalic12pt7b.h"



void setup(void)
{
#ifdef GFX_EXTRA_PRE_INIT
  GFX_EXTRA_PRE_INIT();
#endif
  myPacketSerial.begin(115200);

 
  Serial1.begin(115200, SERIAL_8N1, RXD2, TXD2);
  myPacketSerial.setStream(&Serial1);
  myPacketSerial.setPacketHandler(&onPacketReceived);
  // Init Display
  if (!gfx->begin())
  {
    Serial.println("gfx->begin() failed!");
  }
  gfx->fillScreen(WHITE);

#ifdef GFX_BL
  pinMode(GFX_BL, OUTPUT);
  digitalWrite(GFX_BL, HIGH);
#endif

  gfx->setCursor(10, 30);
  gfx->setFont(&FreeMono8pt7b);
  gfx->setTextSize(2);
  gfx->setTextColor(BLACK);
  gfx->println("CO2:");


  gfx->setCursor(220, 30);
  gfx->setFont(&FreeMono8pt7b);
  gfx->setTextSize(2);
  gfx->setTextColor(BLACK);
  gfx->println("Temperature:");

  gfx->setCursor(220, 220);
  gfx->setFont(&FreeMono8pt7b);
  gfx->setTextSize(2);
  gfx->setTextColor(BLACK);
  gfx->println("Humidity:");

  gfx->setCursor(10, 220);
  gfx->setFont(&FreeMono8pt7b);
  gfx->setTextSize(2);
  gfx->setTextColor(BLACK);
  gfx->println("TVOC:");

  gfx->setCursor(120, 300);
  gfx->setTextSize(2);
  gfx->setTextColor(BLUE);
  gfx->println("ARDUINO DEMO");

  gfx->drawLine(0, 270, 480, 270, BLACK);
  delay(1000); 
}

void loop()
{

  myPacketSerial.update();

  delay(1000); // 1 second
}

void onPacketReceived(const uint8_t *buffer, size_t size) {


  Serial.printf("<--- recv len:%d, data: ", size);
 
  if (size < 1) {
    return;
  }

  float dataval;
  switch (buffer[0]) {
    
    case PKT_TYPE_SENSOR_SCD41_CO2:
      {
        memcpy(&dataval, &buffer[1], sizeof(float));
        Serial.print("CO2 Level:  ");
        Serial.println(dataval);
        gfx->fillRect(10,45,120,30,WHITE);
        gfx->setCursor(10, 70);
        gfx->setFont(&FreeMono8pt7b);
        gfx->setTextColor(RED);
        gfx->println(dataval);

        break;
      }
    default:
      break;
  }
   switch (buffer[0]) {
    case PKT_TYPE_SENSOR_SHT41_TEMP:
      {
        memcpy(&dataval, &buffer[1], sizeof(float));
        Serial.print("sht temp:  ");
        Serial.println(dataval, 2);
        gfx->fillRect(220,45,120,30,WHITE);
        gfx->setCursor(220, 70);
        gfx->setFont(&FreeMono8pt7b);
        gfx->setTextColor(RED);
        gfx->println(dataval);
        break;
      }
    default:
      break;
  }
   switch (buffer[0]) {
    case PKT_TYPE_SENSOR_SHT41_HUMIDITY:
      {
        memcpy(&dataval, &buffer[1], sizeof(float));
        Serial.print("sht humidity:  ");
        Serial.println(dataval, 2);
        gfx->fillRect(220,225,120,30,WHITE);
        gfx->setCursor(220, 250);
        gfx->setFont(&FreeMono8pt7b);
        gfx->setTextColor(RED);
        gfx->println(dataval);
        break;
      }
    default:
      break;
  }
   switch (buffer[0]) {
    case PKT_TYPE_SENSOR_TVOC_INDEX:
      {
        memcpy(&dataval, &buffer[1], sizeof(float));
        Serial.print("TVOC INDEX:  ");
        Serial.println(dataval);
        gfx->fillRect(10,225,120,30,WHITE);
        gfx->setCursor(10, 250);
        gfx->setFont(&FreeMono8pt7b);
        gfx->setTextColor(RED);
        gfx->println(dataval);
        break;
      }
    default:
      break;
  }
}
