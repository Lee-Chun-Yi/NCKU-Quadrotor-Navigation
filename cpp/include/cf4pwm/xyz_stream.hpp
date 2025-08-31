#pragma once

#include <atomic>
#include <cstdint>

namespace cf4pwm {

// Ensure proper alignment even when the TU uses /Zp1 (pack=1)
struct alignas(4) XyzTarget {
  alignas(4) std::atomic<float> x{0.0f};
  alignas(4) std::atomic<float> y{0.0f};
  alignas(4) std::atomic<float> z{0.0f};
};

void start_xyz_stream(const XyzTarget* tgt, const char* host, uint16_t port, double rate_hz);
void stop_xyz_stream();

} // namespace cf4pwm
