#pragma once
#include <driver/i2s.h>

namespace Audio {

  // =======================
  // I2S
  // =======================
  inline constexpr i2s_port_t I2S_PORT = I2S_NUM_0;

  // =======================
  // Audio format
  // =======================
  inline constexpr int SAMPLE_RATE = 44100;
  inline constexpr int AMP         = 1000;

  // =======================
  // Buffers
  // =======================
  inline constexpr int FRAMES_PER_BUFFER = 256;
  inline constexpr int DMA_BUF_COUNT = 8;

} // namespace Audio
