#include <math.h>
#include "config.h"

Audio audio;
Tone tone1;


static constexpr int   kSlotMs = 50;     // whole slot: tone + silent tail
static constexpr int   kToneMs = 25;     // active tone length
static constexpr float kDecay  = 90.0f;  // exp decay rate (1/s) -> sharp click

static constexpr int kSlotSamples = (SAMPLE_RATE * kSlotMs) / 1000;
static constexpr int kToneSamples = (SAMPLE_RATE * kToneMs) / 1000;



I2SStream i2s;

void setupSpeaker(){
  auto config = i2sStream.defaultConfig(TX_MODE);
  config.pin_bck = SPEAKER_BCLK;
  config.pin_ws = SPEAKER_LRCLK;
  config.pin_data = SPEAKER_DOUT;
  // config.pin_mck = 0; // optional master clock pin
  // config.i2s_format = I2S_STD_FORMAT; // default format
  // config.is_master = true; // default esp32 is master
  config.sample_rate = 44100;
  config.channels = 2;
  config.bits_per_sample = 16;
  i2sStream.begin(config);

  renderClick(s_clickNormal, (float)FREQ_NORMAL);
}

static void renderClick(int16_t* buf, float freq) {
  for (int i = 0; i < kSlotSamples; i++) {
    if (i < kToneSamples) {
      float t   = (float)i / (float)SAMPLE_RATE;
      float env = expf(-kDecay * t);
      float s   = sinf(2.0f * PI * freq * t) * env;
      buf[i] = (int16_t)(s * 26000.0f);     // headroom below int16 max
    } else {
      buf[i] = 0;                            // silent tail
    }
  }
}


void playtock(double volume){
  if (volume <= 0.0f) return;
  if (volume > 1.0f)  volume = 1.0f;
const int CHUNK = 256;            // frames per write
  int16_t   frames[CHUNK * 2];      // interleaved L/R

  int i = 0;
  while (i < kSlotSamples) {
    int n = kSlotSamples - i;
    if (n > CHUNK) n = CHUNK;

    for (int j = 0; j < n; j++) {
      int16_t s = (int16_t)(src[i + j] * volume);
      frames[2 * j]     = s;        // left
      frames[2 * j + 1] = s;        // right
    }
    i2sOut.write((uint8_t*)frames, (size_t)n * 2 * sizeof(int16_t));
    i += n;   

}
