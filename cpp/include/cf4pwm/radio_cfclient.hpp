#pragma once

#include <array>
#include <cstdint>
#include <memory>
#include <string>

#include "cf4pwm/msvc_pack_compat.hpp"
#include <crazyflie_cpp/Crazyflie.h>
#include <crazyflie_cpp/Crazyradio.h>

#ifdef _MSC_VER
  #pragma pack(pop)
#endif

namespace cf4pwm {

class RadioClient {
public:
  RadioClient() = default;
  ~RadioClient();

  bool init(const std::string& uri);
  bool send4(uint16_t m1, uint16_t m2, uint16_t m3, uint16_t m4) noexcept;
  void close() noexcept;

private:
  std::unique_ptr<crazyflie_cpp::Crazyflie> cf_;
  std::array<uint8_t, 9> packet_{}; // header + 8 payload bytes
};

} // namespace cf4pwm

