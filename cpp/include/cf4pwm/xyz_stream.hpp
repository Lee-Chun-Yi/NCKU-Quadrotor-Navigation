#pragma once

#include <atomic>
#include <cstdint>

namespace cf4pwm {

struct XyzTarget {
  std::atomic<float> x{0.0f};
  std::atomic<float> y{0.0f};
  std::atomic<float> z{0.0f};
};

void start_xyz_stream(const XyzTarget* tgt, const char* host, uint16_t port, double rate_hz);
void stop_xyz_stream();

} // namespace cf4pwm

