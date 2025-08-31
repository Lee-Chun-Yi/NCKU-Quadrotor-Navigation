// Include Crazyflie class definition before use
#include <crazyflie_cpp/Crazyflie.h>
#include "cf4pwm/radio_cfclient.hpp"

namespace cf4pwm {

RadioClient::RadioClient(std::string uri, uint8_t port, uint8_t channel)
    : uri_(std::move(uri)), port_(port), chan_(channel) {}

RadioClient::~RadioClient() { disconnect(); }

void RadioClient::connect() {
  if (!cf_) {
    cf_ = std::make_unique<Crazyflie>(uri_.c_str());
  }
}

void RadioClient::disconnect() { cf_.reset(); }

bool RadioClient::isConnected() const { return static_cast<bool>(cf_); }

void RadioClient::sendRaw(const uint8_t* data, size_t len) {
  if (!cf_) {
    throw std::runtime_error("Crazyflie not connected");
  }
  cf_->sendPacket(data, static_cast<uint8_t>(len));
}

void RadioClient::send4pwm(uint16_t m1, uint16_t m2, uint16_t m3, uint16_t m4) {
  Pwm4Packet pkt;
  pkt.header = static_cast<uint8_t>((port_ << 4) | (chan_ & 0x0F));
  pkt.m1 = m1;
  pkt.m2 = m2;
  pkt.m3 = m3;
  pkt.m4 = m4;
  sendRaw(reinterpret_cast<const uint8_t*>(&pkt), sizeof(pkt));
}

} // namespace cf4pwm

