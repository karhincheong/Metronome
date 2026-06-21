#include <AudioLogger.h>
#include <AudioTools.h>
#include <AudioToolsConfig.h>

#include <Adafruit_ST7735.h>
#include <Adafruit_GFX.h>
#include <ESP32RotaryEncoder.h>
#include <SPI.h>
#include <driver/i2s.h>
#include <Audio.h>
#include "display.ino"
#include "speaker.ino"
#include "config.h"

/*
from example code
connecting Rotary encoder

Rotary encoder side    MICROCONTROLLER side  
-------------------    ---------------------------------------------------------------------
CLK (A pin)            any microcontroler intput pin with interrupt -> in this example pin 32
DT (B pin)             any microcontroler intput pin with interrupt -> in this example pin 21
SW (button pin)        any microcontroler intput pin with interrupt -> in this example pin 25
GND - to microcontroler GND
VCC                    microcontroler VCC (then set ROTARY_ENCODER_VCC_PIN -1) 

***OR in case VCC pin is not free you can cheat and connect:***
VCC                    any microcontroler output pin - but set also ROTARY_ENCODER_VCC_PIN 25 
                        in this example pin 25

*/



int tempo = 60;
int timesigsel = 0;
int volumesel = 0;

struct timesig {
	int uppper;
	int lower;
};

const timesig SIGS arr[4] = {{1,4}, {2,4}, {3,4}, {4,4}};
const uint8_t SIG_COUNT = sizeof(SIGS) / sizeof(SIGS[0]);

RotaryEncoder re_tempo(TEMPO_A_PIN, TEMPO_B_PIN, TEMPO_BUTTON_PIN);
RotaryEncoder re_timesig(TIMESIG_A_PIN, TIMESIG_B_PIN, TIMESIG_BUTTON_PIN);
RotaryEncoder re_volume(VOLUME_A_PIN, VOLUME_B_PIN, VOLUME_BUTTON_PIN);

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

static void handlePlayPause(){
	play = digitalRead(PAUSEPLAY);
}

void storetempovalue(long value) {
	tempo = value;
	g_uiDirty = true;
}

void storetimesigvalue(long value) {
	timesigsel = value;
	g_uiDirty = true;
}

void storevolumevalue(long value) {
	timesig = value;
	g_uiDirty = true;
}

void setupRotaryEncoders(){
	re_tempo.setBoundaries(30, 180, false);          // 30..250 BPM, no wrap
  re_tempo.onTurned(&storetempovalue);
  re_tempo.setEncoderValue(g_tempo);
  re_tempo.begin();

  re_timesig.setBoundaries(0, SIG_COUNT - 1, true); // wrap around the list
  re_timesig.onTurned(&storetimesigvalue));
  re_timesig.setEncoderValue(g_sigIndex);
  re_timesig.begin();

  re_volume.setBoundaries(0, 10, false);            // 0..10
  re_volume.onTurned(&storevolumevalue);
  re_volume.setEncoderValue(g_volume);
  re_volume.begin();
}

void setup() {
	// put your setup code here, to run once:
	Serial.begin(115200);

	setupRotaryEncoders();
	setupDisplay();
	setupSpeaker();
	volatile bool play = true;
	tft.initR(INITR_GREENTAB);

	// Setup I2S
	audio.setPinout(SPEAKER_BCLK, SPEAKER_LRCLK, SPEAKER_DOUT);

	// Set Volume
	audio.setVolume(5);



	// Open music file
	
	// sine wave modulation for metronome tock
}

void loop() {
	// put your main code here, to run repeatedly:
	double time_between_beat = 60000/tempo;

	if (g_uiDirty) {
    g_uiDirty = false;
    if (g_currentBeat >= SIGS[g_sigIndex].num) g_currentBeat = 0;
    redrawControls();
  }
	 if (g_running) {
    // Tempo is quarter-note BPM. Scale by the lower number so that, if you
    // later add e.g. x/8 signatures, an eighth-note beat is half as long.
    unsigned long interval =
        (60000UL * 4UL) / ((unsigned long)g_tempo * SIGS[g_sigIndex].den);

    unsigned long now = millis();
    if (now - g_lastBeat >= interval) {
      g_lastBeat += interval;                       // keep phase locked...
      if (now - g_lastBeat >= interval) {           // ...but resync if we
        g_lastBeat = now;                           //    fell badly behind
      }

      int  total  = SIGS[g_sigIndex].num;
      bool accent = (g_currentBeat == 0);

      drawBeats(g_currentBeat, total);              // fast SPI update first
      playTock(accent, g_volume / 10.0f);           // then the click

      g_currentBeat = (g_currentBeat + 1) % total;
    }
  } else {
    // Parked on the downbeat while paused.
    g_currentBeat = 0;
    g_lastBeat    = millis();
  }
}


}
