#include "cf4pwm/pack.hpp"

namespace cf4pwm {

uint64_t pack4u16(uint16_t a, uint16_t b, uint16_t c, uint16_t d) noexcept {
  return static_cast<uint64_t>(a) |
         (static_cast<uint64_t>(b) << 16) |
         (static_cast<uint64_t>(c) << 32) |
         (static_cast<uint64_t>(d) << 48);
}

void unpack4u16(uint64_t v, uint16_t &a, uint16_t &b, uint16_t &c, uint16_t &d) noexcept {
  a = static_cast<uint16_t>(v & 0xFFFF);
  b = static_cast<uint16_t>((v >> 16) & 0xFFFF);
  c = static_cast<uint16_t>((v >> 32) & 0xFFFF);
  d = static_cast<uint16_t>((v >> 48) & 0xFFFF);
}

void clamp4u16(uint16_t &a, uint16_t &b, uint16_t &c, uint16_t &d) noexcept {
  auto clamp = [](uint16_t &v) {
    if (v > 65535) v = 65535;
  };
  clamp(a); clamp(b); clamp(c); clamp(d);
}

} // namespace cf4pwm

