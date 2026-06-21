#include <Adafruit_ST7735.h>
#include <Adafruit_GFX.h>

static void setupDisplay() {
  pinMode(TFT_BLK, OUTPUT);
  digitalWrite(TFT_BLK, HIGH);
  SPI.begin(TFT_SCLK, -1, TFT_MOSI, -1);  // remap hardware SPI onto our pins
  tft.initR(INITR_BLACKTAB);              // try GREENTAB/REDTAB if colours look off
  tft.setRotation(1);                     // landscape 160 x 128
  tft.fillScreen(ST77XX_BLACK);
  drawTempo();
  drawTimeSig();
  drawVolume();
  drawRun();
  drawBeats(0, SIGS[g_sigIndex].num);
}

void drawTempo() {
  tft.setCursor(92,45);
  tft.print(String(tempo));

  // paint 14
  tft.setFont(&FreeMono9pt7b);
  tft.setCursor(92, 61);
  tft.print("bpm");

}

void drawRun() {
  if (!g_running){
    tft.drawLine(60, 54, 60, 68, 0xFFFF);
    tft.drawLine(64, 54, 64, 68, 0xFFFF);
    // line 21
  else {
    tft.drawLine(55, 52, 64, 60, 0xFFFF);
    // line 21
    tft.drawLine(56, 67, 64, 61, 0xFFFF);
    // line 20 copy 1
    tft.drawLine(68, 51, 77, 59, 0xFFFF);
    // line 21 copy 1
    tft.drawLine(69, 67, 77, 61, 0xFFFF);
  }
}

void drawTimeSig(){
   tft.setFont(&FreeMonoBold18pt7b);
    tft.setCursor(20, 90);
    tft.print(String(SIGS[g_sigIndex].num));
    // string 17 copy 1
    tft.setCursor(20, 114);
    tft.print(String(SIGS[g_sigIndex].den));
}

void drawBeats(int current, int total) {
  tft.fillRect(0, 84, 160, 26, COL_BG);
  if (total < 1) total = 1;

  const int r   = 8;
  const int gap = 6;
  int totalw = total * (2 * r) + (total - 1) * gap;
  int x = (160 - totalw) / 2 + r;
  int y = 84 + 13;

  for (int i = 0; i < total; i++) {
    uint16_t c;
    if (i == current) c = (i == 0) ? COL_DOWN : COL_BEAT;
    else              c = COL_DIM;
    tft.fillCircle(x, y, r, c);
    tft.drawCircle(x, y, r, COL_TXT);
    x += 2 * r + gap;
  }
}

void redrawControls() {
  drawTempo();
  drawTimeSig();
  drawVolume();
  drawRun();
  drawBeats(g_currentBeat, SIGS[g_sigIndex].num);
}