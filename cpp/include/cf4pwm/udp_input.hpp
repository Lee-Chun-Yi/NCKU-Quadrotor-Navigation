#pragma once

#include <atomic>
#include <cstdint>
#include <thread>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#endif

namespace cf4pwm {

struct PwmState {
  std::atomic<uint64_t> packed{0};
};

class UdpInput {
public:
  enum class Expect { Auto, Float, U16 };

  UdpInput();
  ~UdpInput();

  void configure(uint16_t port, bool debug, Expect expect);
  bool start(PwmState* state);
  void stop();

private:
#ifdef _WIN32
  SOCKET sock_ = INVALID_SOCKET;
#else
  int sock_ = -1;
#endif
  std::thread thread_;
  std::atomic<bool> running_{false};
  PwmState* state_ = nullptr;

  // config
  uint16_t port_ = 8888;
  bool debug_ = false;
  Expect expect_ = Expect::Auto;

  // stats
  std::atomic<uint64_t> drops_{0};
  std::atomic<uint64_t> bytes_{0};
  std::atomic<uint64_t> pkts_total_{0};
  std::atomic<int>      lastlen_{0};
};

} // namespace cf4pwm
