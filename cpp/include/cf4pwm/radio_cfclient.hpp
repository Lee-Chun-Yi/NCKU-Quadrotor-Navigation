#pragma once
#include <cstdint>
#include <memory>
#include <string>
#include <stdexcept>

#include <crazyflie_cpp/Crazyradio.h>

namespace cf4pwm {

#pragma pack(push, 1)
struct Pwm4Packet {
  uint8_t  header;             // CRTP port (high 4 bits) + CRTP channel (low 2 bits in link)
  uint16_t m1, m2, m3, m4;
};
#pragma pack(pop)

class RadioClient {
public:
  explicit RadioClient(std::string uri, uint8_t crtpPort = 0x0A, uint8_t crtpChan = 0);

  void connect();
  void disconnect();

  // Returns true if Crazyradio reported an ACK
  bool send4pwm(uint16_t m1, uint16_t m2, uint16_t m3, uint16_t m4);
  bool isConnected() const { return static_cast<bool>(radio_); }

  const std::string& uri() const { return uri_; }
  int  channel() const { return radioCh_; }   // 2.4GHz channel (e.g. 90)
  int  rate() const { return radioRate_; }    // 2=2M, 1=1M, 0=250K

private:
  static std::string trim(std::string s);
  void parseUri();                               // parse uri_ into devid_/radioCh_/radioRate_
  static uint8_t make_crtp_header(uint8_t port, uint8_t chan);

  // Send a raw packet via Crazyradio and return ACK result
  bool sendRaw(const uint8_t* data, size_t len);

private:
  std::string uri_;

  // Prebuilt CRTP header fields
  uint8_t crtpPort_;
  uint8_t crtpChan_;

  // Parsed from URI
  int devid_ = 0;
  int radioCh_ = 80;
  int radioRate_ = 2; // 2=2M, 1=1M, 0=250K

  std::unique_ptr<Crazyradio> radio_;
};

} // namespace cf4pwm

