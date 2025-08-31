// src/radio_cfclient.cpp
#include "cf4pwm/radio_cfclient.hpp"

#include <crazyflie_cpp/Crazyradio.h>

#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <utility>

namespace cf4pwm {

// ----- helpers declared in header -----
static inline bool is_space(char c) {
  return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v';
}

std::string RadioClient::trim(std::string s) {
  size_t b = 0, e = s.size();
  while (b < e && is_space(s[b])) ++b;
  while (e > b && is_space(s[e - 1])) --e;
  return s.substr(b, e - b);
}

void RadioClient::parseUri() {
  // Supported: radio://<devid>/<rf_channel>/<rate>M
  // Example:   radio://0/90/2M
  std::string u = trim(uri_);
  unsigned devid = 0, ch = static_cast<unsigned>(radioCh_), rateM = static_cast<unsigned>(radioRate_);
  if (u.rfind("radio://", 0) == 0) {
    std::string rest = u.substr(8);
    auto s0 = rest.find('/');
    if (s0 != std::string::npos) {
      if (s0 > 0) devid = static_cast<unsigned>(std::stoul(rest.substr(0, s0)));
      rest = rest.substr(s0 + 1);
      auto s1 = rest.find('/');
      if (s1 != std::string::npos) {
        if (s1 > 0) ch = static_cast<unsigned>(std::stoul(rest.substr(0, s1)));
        rest = rest.substr(s1 + 1);
        if (!rest.empty()) {
          if (rest.back() == 'M' || rest.back() == 'm') rest.pop_back();
          if (!rest.empty()) rateM = static_cast<unsigned>(std::stoul(rest));
        }
      } else {
        if (!rest.empty()) ch = static_cast<unsigned>(std::stoul(rest));
      }
    }
  }
  devid_     = static_cast<int>(devid);
  radioCh_   = static_cast<int>(ch);
  // Map rateM: 0->250K, 1->1M, 2->2M
  radioRate_ = static_cast<int>((rateM == 0) ? 0 : (rateM == 1 ? 1 : 2));
}

uint8_t RadioClient::make_crtp_header(uint8_t port, uint8_t chan) {
  // bit layout: [port:4][link:2][chan:2], set link to 3
  return static_cast<uint8_t>((port << 4) | (3u << 2) | (chan & 0x3u));
}

// ----- RadioClient impl -----
RadioClient::RadioClient(std::string uri, uint8_t crtpPort, uint8_t crtpChan)
  : uri_(std::move(uri)), crtpPort_(crtpPort), crtpChan_(crtpChan) {
  parseUri();
}

void RadioClient::connect() {
  radio_ = std::make_unique<Crazyradio>(static_cast<uint32_t>(devid_));
  if (!radio_) throw std::runtime_error("Failed to create Crazyradio");

  radio_->setChannel(static_cast<uint8_t>(radioCh_));

  Crazyradio::Datarate dr = Crazyradio::Datarate_2MPS;
  switch (radioRate_) {
    case 0: dr = Crazyradio::Datarate_250KPS; break;
    case 1: dr = Crazyradio::Datarate_1MPS;   break;
    case 2: default: dr = Crazyradio::Datarate_2MPS; break;
  }
  radio_->setDatarate(dr);

  std::cerr << "[radio] connected (ch=" << radioCh_
            << ", rate=" << radioRate_ << ")\n";
}

void RadioClient::disconnect() {
  if (radio_) {
    std::cerr << "[radio] disconnected\n";
    radio_.reset();
  }
}

void RadioClient::sendRaw(const uint8_t* data, size_t len) {
  if (!radio_) throw std::runtime_error("RadioClient: not connected");
  ITransport::Ack ack; // collect ack but we don't parse it here
  radio_->sendPacket(data, static_cast<uint32_t>(len), ack);
}

void RadioClient::send4pwm(uint16_t m1, uint16_t m2, uint16_t m3, uint16_t m4) {
  // Build CRTP packet matching Python sender: port=0x0A, channel=0, data=<HHHH>
  Pwm4Packet pkt{};
  pkt.header = make_crtp_header(0x0A, 0);
  pkt.m1 = m1; pkt.m2 = m2; pkt.m3 = m3; pkt.m4 = m4;
  sendRaw(reinterpret_cast<const uint8_t*>(&pkt), sizeof(pkt));
}

} // namespace cf4pwm
