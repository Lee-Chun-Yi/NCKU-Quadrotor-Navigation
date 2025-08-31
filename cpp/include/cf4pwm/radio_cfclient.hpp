#pragma once
#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>

// Pull in the full Crazyflie class definition
#include <crazyflie_cpp/Crazyflie.h>

namespace cf4pwm {

#pragma pack(push, 1)
struct Pwm4Packet {
  uint8_t header;
  uint16_t m1;
  uint16_t m2;
  uint16_t m3;
  uint16_t m4;
};

class RadioClient {
public:
  explicit RadioClient(std::string uri, uint8_t port = 0x0A, uint8_t channel = 0);
  ~RadioClient();

  void connect();
  void disconnect();
  void send4pwm(uint16_t m1, uint16_t m2, uint16_t m3, uint16_t m4);
  void sendRaw(const uint8_t* data, size_t len);
  bool isConnected() const;

private:
  std::string uri_;
  uint8_t port_;
  uint8_t chan_;
  std::unique_ptr<Crazyflie> cf_;  // note: no crazyflie_cpp namespace
};

} // namespace cf4pwm

// Balance any upstream pack(push,1) if used in our code
#include <cf4pwm/msvc_pack_pop.hpp>
