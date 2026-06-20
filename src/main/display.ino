#include <Adafruit_ST7735.h>
#include <Adafruit_GFX.h>

static void setupDisplay() {
  pinMode(TFT_BLK, OUTPUT); 
  digitalWrite(TFT_BLK, HIGH);
  SPI.begin(TFT_SCLK, -1, TFT_MOSI, -1);    // remap hardware SPI onto our pins
  tft.initR(INITR_BLACKTAB);                // try GREENTAB/REDTAB if colours look off
  tft.setRotation(1);                       // landscape 160 x 128
  tft.fillScreen(ST77XX_BLACK);
  drawTempo(); drawTimeSig(); drawVolume(); drawRun();
  drawBeats(0, SIGS[g_sigIndex].num);
}
