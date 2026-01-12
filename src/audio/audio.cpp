#include "audio/audio.h"

#include <Arduino.h>
#include <driver/i2s.h>
#include <math.h>

#include "system/settings_pins.h"
#include "system/settings_audio.h"

// If PI isn't defined in your environment for some reason, keep this.
#ifndef PI
  #define PI 3.14159265358979323846
#endif

namespace AudioOut {

  // Keep buffers private to this module
  static int16_t s_stereoBuf[Audio::FRAMES_PER_BUFFER * 2];

  static void setupI2S() {
    i2s_config_t cfg = {
      .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
      .sample_rate = Audio::SAMPLE_RATE,
      .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
      .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,

      // Avoid deprecated warning on newer cores, keep compatibility on older cores
#if defined(I2S_COMM_FORMAT_STAND_I2S)
      .communication_format = I2S_COMM_FORMAT_STAND_I2S,
#else
      .communication_format = I2S_COMM_FORMAT_I2S,
#endif

      .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
      .dma_buf_count = Audio::DMA_BUF_COUNT,
      .dma_buf_len   = Audio::FRAMES_PER_BUFFER,
      .use_apll = false,
      .tx_desc_auto_clear = true,
      .fixed_mclk = 0
    };

    i2s_pin_config_t pins = {};
    pins.bck_io_num   = Pins::I2S_BCLK;
    pins.ws_io_num    = Pins::I2S_LRC;
    pins.data_out_num = Pins::I2S_DOUT;
    pins.data_in_num  = I2S_PIN_NO_CHANGE;

    ESP_ERROR_CHECK(i2s_driver_install(Audio::I2S_PORT, &cfg, 0, nullptr));
    ESP_ERROR_CHECK(i2s_set_pin(Audio::I2S_PORT, &pins));
    ESP_ERROR_CHECK(i2s_zero_dma_buffer(Audio::I2S_PORT));
  }

  void begin() {
    setupI2S();
  }

  void playToneHz(float hz, uint16_t ms) {
    if (hz <= 0.0f || ms == 0) { delay(ms); return; }

    float phase = 0.0f;
    const float inc = 2.0f * PI * hz / (float)Audio::SAMPLE_RATE;

    uint32_t totalSamples = (uint32_t)((Audio::SAMPLE_RATE * (uint32_t)ms) / 1000U);

    while (totalSamples > 0) {
      uint32_t framesThis = (totalSamples > (uint32_t)Audio::FRAMES_PER_BUFFER)
        ? (uint32_t)Audio::FRAMES_PER_BUFFER
        : totalSamples;

      for (uint32_t i = 0; i < framesThis; i++) {
        int16_t s = (int16_t)(sinf(phase) * Audio::AMP);
        s_stereoBuf[i * 2 + 0] = s;
        s_stereoBuf[i * 2 + 1] = s;

        phase += inc;
        if (phase >= 2.0f * PI) phase -= 2.0f * PI;
      }

      size_t written = 0;
      i2s_write(Audio::I2S_PORT,
                s_stereoBuf,
                framesThis * 2 * sizeof(int16_t),
                &written,
                portMAX_DELAY);

      totalSamples -= framesThis;
    }
  }

  static void playBeepPattern(const float *notes, const uint16_t *durMs, size_t count, uint16_t gapMs = 30) {
    for (size_t i = 0; i < count; i++) {
      playToneHz(notes[i], durMs[i]);
      delay(gapMs);
    }
  }

  void playBtn1Sound() {
    static const float notes[] = { 880.0f };
    static const uint16_t dur[] = { 80 };
    playBeepPattern(notes, dur, 1);
  }

  void playBtn2Sound() {
    static const float notes[] = { 660.0f, 660.0f };
    static const uint16_t dur[] = { 60, 60 };
    playBeepPattern(notes, dur, 2, 40);
  }

} // namespace AudioOut
