#pragma once

#include <cstdint>

namespace cf4pwm {

uint64_t pack4u16(uint16_t a, uint16_t b, uint16_t c, uint16_t d) noexcept;
void unpack4u16(uint64_t v, uint16_t &a, uint16_t &b, uint16_t &c, uint16_t &d) noexcept;
void clamp4u16(uint16_t &a, uint16_t &b, uint16_t &c, uint16_t &d) noexcept;

} // namespace cf4pwm

