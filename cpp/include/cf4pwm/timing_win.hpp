#pragma once

#include <cstdint>

namespace cf4pwm {

struct RtConfig {
  bool realtime{false};
  unsigned long long affinityMask{0};
  bool spinTail{false};
};

void init_timing(const RtConfig& cfg);
void restore_timing();
int64_t qpc_now();
int64_t qpc_freq();
int64_t secondsToTicks(double s, int64_t fq);
double ticksToMs(int64_t t, int64_t fq);
int64_t ticksToNs(int64_t t, int64_t fq);

} // namespace cf4pwm

