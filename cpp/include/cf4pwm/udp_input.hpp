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
  UdpInput();
  ~UdpInput();
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
};

} // namespace cf4pwm

