#include "cf4pwm/xyz_stream.hpp"

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
#endif

#include <atomic>
#include <chrono>
#include <cstring>
#include <thread>

namespace cf4pwm {

namespace {
#ifdef _WIN32
SOCKET g_sock = INVALID_SOCKET;
#else
int g_sock = -1;
#endif
std::thread g_thr;
std::atomic<bool> g_running{false};
}

void start_xyz_stream(const XyzTarget* tgt, const char* host, uint16_t port, double rate_hz) {
  if (g_running.load(std::memory_order_acquire)) return;

#ifdef _WIN32
  WSADATA wsa;
  WSAStartup(MAKEWORD(2, 2), &wsa);
  g_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (g_sock == INVALID_SOCKET) {
    WSACleanup();
    return;
  }

  sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  inet_pton(AF_INET, host, &addr.sin_addr);

  if (rate_hz <= 0.0) rate_hz = 1.0;
  const auto period = std::chrono::duration<double>(1.0 / rate_hz);

  g_running.store(true, std::memory_order_release);
  g_thr = std::thread([tgt, addr, period]() {
    sockaddr_in dst = addr;
    auto next = std::chrono::steady_clock::now();
    while (g_running.load(std::memory_order_relaxed)) {
      next += std::chrono::duration_cast<std::chrono::steady_clock::duration>(period);

      float xf = tgt->x.load(std::memory_order_relaxed);
      float yf = tgt->y.load(std::memory_order_relaxed);
      float zf = tgt->z.load(std::memory_order_relaxed);
      uint8_t buf[12];
      std::memcpy(buf + 0, &xf, 4);
      std::memcpy(buf + 4, &yf, 4);
      std::memcpy(buf + 8, &zf, 4);
      sendto(g_sock, reinterpret_cast<const char*>(buf), 12, 0,
             reinterpret_cast<const sockaddr*>(&dst), sizeof(dst));

      std::this_thread::sleep_until(next);
      auto now = std::chrono::steady_clock::now();
      if (now > next + std::chrono::seconds(1)) {
        next = now; // skip ahead to avoid drift
      }
    }
  });
#else
  (void)tgt; (void)host; (void)port; (void)rate_hz;
#endif
}

void stop_xyz_stream() {
#ifdef _WIN32
  g_running.store(false, std::memory_order_release);
  if (g_thr.joinable()) g_thr.join();
  if (g_sock != INVALID_SOCKET) {
    closesocket(g_sock);
    g_sock = INVALID_SOCKET;
  }
  WSACleanup();
#endif
}

} // namespace cf4pwm

