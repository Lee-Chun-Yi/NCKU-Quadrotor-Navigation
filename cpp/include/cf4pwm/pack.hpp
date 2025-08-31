#pragma once
#include <cstdint>
#include <array>

namespace cf4pwm {

// 小端序 8 bytes
using Bytes8 = std::array<std::uint8_t, 8>;

/* 將四個 uint16 以小端序打包到 uint64_t（a 最低位，d 最高位） */
std::uint64_t pack4u16(std::uint16_t a, std::uint16_t b,
                       std::uint16_t c, std::uint16_t d) noexcept;

/* 從 uint64_t（小端序）解包為四個 uint16 */
void unpack4u16(std::uint64_t v,
                std::uint16_t& a, std::uint16_t& b,
                std::uint16_t& c, std::uint16_t& d) noexcept;

/* 將四個 uint16 以小端序打包到 8 bytes 陣列 */
void pack4u16(std::uint16_t a, std::uint16_t b,
              std::uint16_t c, std::uint16_t d,
              Bytes8& out) noexcept;

/* 從 8 bytes（小端序）解回四個 uint16 */
void unpack4u16(const Bytes8& in,
                std::uint16_t& a, std::uint16_t& b,
                std::uint16_t& c, std::uint16_t& d) noexcept;

/* 將四個值限制在 0..65535（uint16_t 本身即此範圍；此函式供介面一致） */
void clamp4u16(std::uint16_t& a, std::uint16_t& b,
               std::uint16_t& c, std::uint16_t& d) noexcept;

} // namespace cf4pwm
