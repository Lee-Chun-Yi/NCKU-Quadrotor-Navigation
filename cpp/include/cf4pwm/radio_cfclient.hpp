#pragma once

#include <array>
#include <cstdint>
#include <memory>
#include <string>

namespace crazyflie_cpp {
class Crazyflie;
}

namespace cf4pwm {

class RadioClient {
public:
  bool init(const std::string& uri);
  bool send4(uint16_t m1, uint16_t m2, uint16_t m3, uint16_t m4) noexcept;
  void close() noexcept;

private:
  std::unique_ptr<crazyflie_cpp::Crazyflie> cf_;
  std::array<uint8_t, 9> packet_{}; // header + 8 payload bytes
};

} // namespace cf4pwm

