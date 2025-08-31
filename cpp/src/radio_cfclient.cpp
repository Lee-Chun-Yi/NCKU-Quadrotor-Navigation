#pragma once
#include <cstdint>

namespace cf4pwm {

// 只在自己的結構上啟用 1-byte 對齊
#pragma pack(push, 1)
struct CfClientHeader {
    uint8_t  port;
    uint8_t  channel;
    uint8_t  id;
    uint8_t  reserved;
};

struct CfClientPwmPayload {
    uint16_t m0;
    uint16_t m1;
    uint16_t m2;
    uint16_t m3;
};
#pragma pack(pop)

// 你的工具：把 4x u16 從 bytes 解出來
inline void unpack4u16(const uint8_t* src, uint16_t& a, uint16_t& b, uint16_t& c, uint16_t& d) {
    auto rd = [&](int i)->uint16_t {
        return static_cast<uint16_t>(src[i] | (static_cast<uint16_t>(src[i+1]) << 8));
    };
    a = rd(0); b = rd(2); c = rd(4); d = rd(6);
}

} // namespace cf4pwm
