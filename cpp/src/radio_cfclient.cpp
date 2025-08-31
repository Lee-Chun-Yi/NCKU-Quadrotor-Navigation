#include "cf4pwm/radio_cfclient.hpp"

namespace cf4pwm {

RadioClient::~RadioClient() = default;

bool RadioClient::init(const std::string& uri) {
  cf_ = std::make_unique<crazyflie_cpp::Crazyflie>(uri.c_str());
  packet_[0] = 0xA0; // port 0x0A channel 0
  return true;
}

bool RadioClient::send4(uint16_t m1, uint16_t m2, uint16_t m3, uint16_t m4) noexcept {
  if (!cf_)
    return false;
  packet_[1] = static_cast<uint8_t>(m1 & 0xFF);
  packet_[2] = static_cast<uint8_t>(m1 >> 8);
  packet_[3] = static_cast<uint8_t>(m2 & 0xFF);
  packet_[4] = static_cast<uint8_t>(m2 >> 8);
  packet_[5] = static_cast<uint8_t>(m3 & 0xFF);
  packet_[6] = static_cast<uint8_t>(m3 >> 8);
  packet_[7] = static_cast<uint8_t>(m4 & 0xFF);
  packet_[8] = static_cast<uint8_t>(m4 >> 8);
  try {
    cf_->sendPacket(packet_.data(), static_cast<uint8_t>(packet_.size()));
    return true;
  } catch (...) {
    return false;
  }
}

void RadioClient::close() noexcept { cf_.reset(); }

} // namespace cf4pwm

