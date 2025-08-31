#include "cf4pwm/pack.hpp"

namespace cf4pwm {

std::uint64_t pack4u16(std::uint16_t a, std::uint16_t b,
                       std::uint16_t c, std::uint16_t d) noexcept {
  // 小端序：a -> bits [15:0], b -> [31:16], c -> [47:32], d -> [63:48]
  return static_cast<std::uint64_t>(a)
       | (static_cast<std::uint64_t>(b) << 16)
       | (static_cast<std::uint64_t>(c) << 32)
       | (static_cast<std::uint64_t>(d) << 48);
}

void unpack4u16(std::uint64_t v,
                std::uint16_t& a, std::uint16_t& b,
                std::uint16_t& c, std::uint16_t& d) noexcept {
  a = static_cast<std::uint16_t>( v        & 0xFFFFu);
  b = static_cast<std::uint16_t>((v >> 16) & 0xFFFFu);
  c = static_cast<std::uint16_t>((v >> 32) & 0xFFFFu);
  d = static_cast<std::uint16_t>((v >> 48) & 0xFFFFu);
}

void pack4u16(std::uint16_t a, std::uint16_t b,
              std::uint16_t c, std::uint16_t d,
              Bytes8& out) noexcept {
  out[0] = static_cast<std::uint8_t>( a        & 0xFFu);
  out[1] = static_cast<std::uint8_t>((a >> 8)  & 0xFFu);
  out[2] = static_cast<std::uint8_t>( b        & 0xFFu);
  out[3] = static_cast<std::uint8_t>((b >> 8)  & 0xFFu);
  out[4] = static_cast<std::uint8_t>( c        & 0xFFu);
  out[5] = static_cast<std::uint8_t>((c >> 8)  & 0xFFu);
  out[6] = static_cast<std::uint8_t>( d        & 0xFFu);
  out[7] = static_cast<std::uint8_t>((d >> 8)  & 0xFFu);
}

void unpack4u16(const Bytes8& in,
                std::uint16_t& a, std::uint16_t& b,
                std::uint16_t& c, std::uint16_t& d) noexcept {
  a = static_cast<std::uint16_t>(in[0] | (static_cast<std::uint16_t>(in[1]) << 8));
  b = static_cast<std::uint16_t>(in[2] | (static_cast<std::uint16_t>(in[3]) << 8));
  c = static_cast<std::uint16_t>(in[4] | (static_cast<std::uint16_t>(in[5]) << 8));
  d = static_cast<std::uint16_t>(in[6] | (static_cast<std::uint16_t>(in[7]) << 8));
}

void clamp4u16(std::uint16_t& a, std::uint16_t& b,
               std::uint16_t& c, std::uint16_t& d) noexcept {
  // uint16_t 天生已在 0..65535，這裡保留接口（若未來改型別時仍適用）
  // 若你會從 float/int 轉型，可在呼叫前自行 clamp 後再轉 uint16_t。
  (void)a; (void)b; (void)c; (void)d;
}

} // namespace cf4pwm
