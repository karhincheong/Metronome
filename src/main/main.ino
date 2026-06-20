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

#define TEMPO_A_PIN 4
#define TEMPO_B_PIN 5
#define TEMPO_BUTTON_PIN 1

#define TIMESIG_A_PIN 14
#define TIMESIG_B_PIN 15
#define TIMESIG_BUTTON_PIN 2

#define VOLUME_A_PIN 17
#define VOLUME_A_PIN 18
#define VOLUME_BUTTON_PIN 10

#define TFT_CS 38
#define TFT_DC 39
#define TFT_RST 40
#define TFT_SDA 41
#define TFT_CLK 42

#define PAUSEPLAY 6

#define SPEAKER_DOUT 7  //DIN on MAX39857A
#define SPEAKER_BCLK 8
#define SPEAKER_LRCLK 9

#define SAMPLE_RATE 44100
#define I2S_NUM I2S_NUM_0

#define FREQ 440 //tock frequency

RotaryEncoder re_tempo(TEMPO_A_PIN, TEMPO_B_PIN, TEMPO_BUTTON_PIN);
RotaryEncoder re_timesig(TIMESIG_A_PIN, TIMESIG_B_PIN, TIMESIG_BUTTON_PIN);
RotaryEncoder re_volume(VOLUME_A_PIN, VOLUME_B_PIN, VOLUME_BUTTON_PIN);

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

int tempo = 60;
int timesigsel = 0;
int volumesel = 0;

struct timesig {
	int uppper;
	int lower;
};

struct timesigs arr[4] = {{1,4}, {2,4}, {3,4}, {4,4}};


void storetempovalue(long value) {
	tempo = value;
}

void storetimesigvalue(long value) {
	timesigsel = value;
}

void storevolumevalue(long value) {
	timesig = value;
}

void setup_re_tempo() {
	re_tempo.setBoundaries(10, 200, false);

	// The function specified here will be called every time the knob is turned
	// and the current value will be passed to it
	re_tempo.onTurned(&storetempoValue);

	// The function specified here will be called every time the button is pushed and
	// the duration (in milliseconds) that the button was down will be passed to it
	//re_tempo.onPressed( &button1ToggleRE2 );

	re_tempo.setEncoderValue(60);

	// This is where the inputs are configured and the interrupts get attached
	re_tempo.begin();
}

void setup_re_timesig() {
	re_tempo.setBoundaries(0, 3, false);  //temp as of now considering 1/4, 2/4, 3/4, 4/4 only

	// The function specified here will be called every time the knob is turned
	// and the current value will be passed to it
	re_tempo.onTurned(&storetimesigValue);

	// The function specified here will be called every time the button is pushed and
	// the duration (in milliseconds) that the button was down will be passed to it
	//re_tempo.onPressed( &button1ToggleRE2 );

	re_tempo.setEncoderValue(3);

	// This is where the inputs are configured and the interrupts get attached
	re_tempo.begin();
}

void setup_re_volume() {
	re_tempo.setBoundaries(0, 2, true);  //temp as of now only considering quaver, crochet, minim only

	// The function specified here will be called every time the knob is turned
	// and the current value will be passed to it
	re_tempo.onTurned(&storevolumeValue);

	// The function specified here will be called every time the button is pushed and
	// the duration (in milliseconds) that the button was down will be passed to it
	//re_tempo.onPressed( &button1ToggleRE2 );

	re_tempo.setEncoderValue(60);

	// This is where the inputs are configured and the interrupts get attached
	re_tempo.begin();
}

void setup() {
	// put your setup code here, to run once:
	Serial.begin(9600);

	setup_re_tempo();
	setup_re_timesig();
	setup_re_volume();
	volatile bool pause = true;
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

	setupSpeaker();
}
