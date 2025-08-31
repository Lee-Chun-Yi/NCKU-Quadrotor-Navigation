#include "cf4pwm/udp_input.hpp"
#include "cf4pwm/pack.hpp"

#ifdef _WIN32
#include <windows.h>
#endif

#include <atomic>
#include <chrono>
#include <cmath>
#include <cstring>
#include <iostream>

namespace cf4pwm {

UdpInput::UdpInput() {}
UdpInput::~UdpInput() { stop(); }

void UdpInput::configure(uint16_t port, bool debug, Expect expect) {
  port_ = port;
  debug_ = debug;
  expect_ = expect;
}

bool UdpInput::start(PwmState* state) {
#ifdef _WIN32
  state_ = state;

  static std::atomic<bool> wsa_inited{false};
  if (!wsa_inited.load(std::memory_order_acquire)) {
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
      return false;
    }
    wsa_inited.store(true, std::memory_order_release);
  }

  sock_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (sock_ == INVALID_SOCKET)
    return false;

  // SO_REUSEADDR = 1
  BOOL yes = TRUE;
  setsockopt(sock_, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&yes), sizeof(yes));

  // Increase receive buffer to >= 1MB
  int rcvbuf = 1 << 20;
  setsockopt(sock_, SOL_SOCKET, SO_RCVBUF, reinterpret_cast<const char*>(&rcvbuf), sizeof(rcvbuf));

  sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY; // 0.0.0.0
  addr.sin_port = htons(port_);
  if (bind(sock_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == SOCKET_ERROR) {
    closesocket(sock_);
    sock_ = INVALID_SOCKET;
    return false;
  }

  // non-blocking
  u_long mode = 1;
  ioctlsocket(sock_, FIONBIO, &mode);

  running_ = true;
  drops_.store(0, std::memory_order_relaxed);
  bytes_.store(0, std::memory_order_relaxed);
  pkts_total_.store(0, std::memory_order_relaxed);
  lastlen_.store(0, std::memory_order_relaxed);

  thread_ = std::thread([this]() {
    using clock = std::chrono::steady_clock;
    auto lastStat = clock::now();
    auto lastPktTime = clock::now();
    uint64_t pkts_this_sec = 0;
    bool first_ok_printed = false;
    bool warned_invalid_len = false;
    bool warned_expect_mismatch = false;

    uint8_t buf[2048];
    while (running_.load(std::memory_order_relaxed)) {
      int ret = recv(sock_, reinterpret_cast<char*>(buf), sizeof(buf), 0);
      if (ret == SOCKET_ERROR) {
        int err = WSAGetLastError();
        if (err == WSAEWOULDBLOCK) {
          // periodic stats
          auto now = clock::now();
          if (debug_ && now - lastStat >= std::chrono::seconds(1)) {
            if (now - lastPktTime > std::chrono::seconds(1)) {
              std::cerr << "UDP: no data (timeout)\n";
            } else {
              std::cerr << "UDP: pkts=" << pkts_this_sec
                        << "/s bytes=" << bytes_.load(std::memory_order_relaxed)
                        << " lastlen=" << lastlen_.load(std::memory_order_relaxed)
                        << " drops=" << drops_.load(std::memory_order_relaxed)
                        << "\n";
            }
            lastStat = now;
            pkts_this_sec = 0;
          }
          Sleep(1);
          continue;
        } else {
          // unexpected error -> break
          break;
        }
      }
      if (ret == 0) {
        // ignore
        continue;
      }

      lastlen_.store(ret, std::memory_order_relaxed);
      bytes_.fetch_add(static_cast<uint64_t>(ret), std::memory_order_relaxed);

      const bool len_is_8 = (ret == 8);
      const bool len_is_16 = (ret == 16);

      auto accept_len = [this, len_is_8, len_is_16]() -> bool {
        switch (expect_) {
          case Expect::Auto:  return len_is_8 || len_is_16;
          case Expect::Float: return len_is_16;
          case Expect::U16:   return len_is_8;
        }
        return false;
      }();

      if (!accept_len) {
        drops_.fetch_add(1, std::memory_order_relaxed);
        if (debug_ && !warned_expect_mismatch) {
          std::cerr << "UDP: drop length=" << ret << " (expect mode mismatch)\n";
          warned_expect_mismatch = true;
        }
        continue;
      }

      if (!(len_is_8 || len_is_16)) {
        drops_.fetch_add(1, std::memory_order_relaxed);
        if (debug_ && !warned_invalid_len) {
          std::cerr << "UDP: drop length=" << ret << " (unsupported)\n";
          warned_invalid_len = true;
        }
        continue;
      }

      if (!first_ok_printed && debug_) {
        if (len_is_16) {
          std::cerr << "UDP[" << port_ << "]: first packet ok, format=float32 (16 bytes)\n";
        } else {
          std::cerr << "UDP[" << port_ << "]: first packet ok, format=uint16 (8 bytes)\n";
        }
        first_ok_printed = true;
      }

      if (len_is_8) {
        uint16_t m1, m2, m3, m4;
        // decode little-endian
        m1 = static_cast<uint16_t>(buf[0] | (static_cast<uint16_t>(buf[1]) << 8));
        m2 = static_cast<uint16_t>(buf[2] | (static_cast<uint16_t>(buf[3]) << 8));
        m3 = static_cast<uint16_t>(buf[4] | (static_cast<uint16_t>(buf[5]) << 8));
        m4 = static_cast<uint16_t>(buf[6] | (static_cast<uint16_t>(buf[7]) << 8));
        const uint64_t v = pack4u16(m1, m2, m3, m4);
        state_->packed.store(v, std::memory_order_release);
      } else /* len_is_16 */ {
        float f[4];
        std::memcpy(f, buf, 16);
        uint16_t m[4];
        for (int i = 0; i < 4; ++i) {
          float v = f[i];
          if (!std::isfinite(v)) v = 0.0f;
          if (v < 0.0f) v = 0.0f;
          if (v > 65535.0f) v = 65535.0f;
          m[i] = static_cast<uint16_t>(v + 0.5f);
        }
        const uint64_t v = pack4u16(m[0], m[1], m[2], m[3]);
        state_->packed.store(v, std::memory_order_release);
      }

      pkts_this_sec++;
      pkts_total_.fetch_add(1, std::memory_order_relaxed);
      lastPktTime = clock::now();

      // periodic stats (also print immediately if a second elapsed)
      auto now = clock::now();
      if (debug_ && now - lastStat >= std::chrono::seconds(1)) {
        std::cerr << "UDP: pkts=" << pkts_this_sec
                  << "/s bytes=" << bytes_.load(std::memory_order_relaxed)
                  << " lastlen=" << lastlen_.load(std::memory_order_relaxed)
                  << " drops=" << drops_.load(std::memory_order_relaxed)
                  << "\n";
        lastStat = now;
        pkts_this_sec = 0;
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
  if (sock_ != INVALID_SOCKET) {
    closesocket(sock_);
    sock_ = INVALID_SOCKET;
  }
  if (thread_.joinable())
    thread_.join();
#endif
}

} // namespace cf4pwm
