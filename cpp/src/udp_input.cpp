#include "cf4pwm/udp_input.hpp"
#include "cf4pwm/pack.hpp"

#ifdef _WIN32
#include <windows.h>
#endif
#include <cstring>

namespace cf4pwm {

UdpInput::UdpInput() {}
UdpInput::~UdpInput() { stop(); }

bool UdpInput::start(PwmState* state) {
#ifdef _WIN32
  state_ = state;
  WSADATA wsa;
  if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    return false;

  sock_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (sock_ == INVALID_SOCKET)
    return false;

  sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(8888);
  if (bind(sock_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == SOCKET_ERROR)
    return false;

  u_long mode = 1;
  ioctlsocket(sock_, FIONBIO, &mode);

  running_ = true;
  thread_ = std::thread([this]() {
    uint8_t buf[16];
    while (running_) {
      int ret = recv(sock_, reinterpret_cast<char*>(buf), sizeof(buf), 0);
      if (ret == SOCKET_ERROR) {
        int err = WSAGetLastError();
        if (err == WSAEWOULDBLOCK) {
          Sleep(1);
          continue;
        } else {
          break;
        }
      }
      if (ret == 8) {
        uint16_t m1 = buf[0] | (buf[1] << 8);
        uint16_t m2 = buf[2] | (buf[3] << 8);
        uint16_t m3 = buf[4] | (buf[5] << 8);
        uint16_t m4 = buf[6] | (buf[7] << 8);
        uint64_t packed = pack4u16(m1, m2, m3, m4);
        state_->packed.store(packed, std::memory_order_release);
      } else if (ret == 16) {
        float f[4];
        std::memcpy(f, buf, 16);
        uint16_t m[4];
        for (int i = 0; i < 4; ++i) {
          float v = f[i];
          if (v < 0.f)
            v = 0.f;
          if (v > 65535.f)
            v = 65535.f;
          m[i] = static_cast<uint16_t>(v);
        }
        uint64_t packed = pack4u16(m[0], m[1], m[2], m[3]);
        state_->packed.store(packed, std::memory_order_release);
      }
    }
  });
  return true;
#else
  (void)state;
  return false;
#endif
}

void UdpInput::stop() {
#ifdef _WIN32
  running_ = false;
  if (thread_.joinable())
    thread_.join();
  if (sock_ != INVALID_SOCKET) {
    closesocket(sock_);
    sock_ = INVALID_SOCKET;
  }
  WSACleanup();
#endif
}

} // namespace cf4pwm

