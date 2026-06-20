#include <AudioLogger.h>
#include <AudioTools.h>
#include <AudioToolsConfig.h>

/*
There isn't too much time rn to think about damping sine waves so the metronome tock is just
gonna be from a standard frequency
*/

Audio audio;
Tone tone1;

AudioInfo info(44100, 2, 16);
SineGenerator<int16_t> sineWave;                       // subclass of SoundGenerator with max amplitude of 32000
GeneratedSoundStream<int16_t> sound(sineWave);             // Stream generated from sine wave

I2SStream i2s;

void setupSpeaker(){
  auto config = i2sStream.defaultConfig(TX_MODE);
  config.pin_bck = SPEAKER_BCLK;
  config.pin_ws = SPEAKER_LRCLK;
  config.pin_data = SPEAKER_DOUT;
  // config.pin_mck = 0; // optional master clock pin
  // config.i2s_format = I2S_STD_FORMAT; // default format
  // config.is_master = true; // default esp32 is master

  i2sStream.begin(config);
}

void playtock(double volume){
    

}
