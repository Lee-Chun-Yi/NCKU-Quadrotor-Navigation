// main_win.cpp
#include "cf4pwm/metrics.hpp"
#include "cf4pwm/timing_win.hpp"
#include "cf4pwm/udp_input.hpp"
#include "cf4pwm/radio_cfclient.hpp"

#include <atomic>
#include <cstdint>
#include <algorithm>
#include <chrono>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>

#ifdef _WIN32
#  include <windows.h>
#endif

using namespace std::chrono_literals;
using namespace cf4pwm;

// ---- 本地版 unpack4u16 --------------------------------------------------
// 以 64-bit packed: [w1|w2|w3|w4] (每個16-bit, 低位在右) 取出 4 個 uint16_t
static inline void unpack4u16(std::uint64_t packed,
                              std::uint16_t& w1,
                              std::uint16_t& w2,
                              std::uint16_t& w3,
                              std::uint16_t& w4) noexcept {
  w1 = static_cast<std::uint16_t>( packed        & 0xFFFFu);
  w2 = static_cast<std::uint16_t>((packed >> 16) & 0xFFFFu);
  w3 = static_cast<std::uint16_t>((packed >> 32) & 0xFFFFu);
  w4 = static_cast<std::uint16_t>((packed >> 48) & 0xFFFFu);
}

// ---- Ctrl+C handling -----------------------------------------------------
static std::atomic<bool> g_running{true};

#ifdef _WIN32
static BOOL WINAPI consoleHandler(DWORD) {
  g_running = false;
  return TRUE;
}
#else
static inline void Sleep(unsigned long) {}
#endif

int main(int argc, char** argv) {
  // --- CLI options --------------------------------------------------------
  std::string uri = "radio://0/80/2M";
  double rate_hz = 500.0;
  std::string affinityStr;   // e.g. "3" or "3,5"
  bool realtime = false;     // --prio realtime
  bool spinTail = false;     // busy-wait 到期末
  const char* csvPath = nullptr; // --csv <file>

  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    if (arg == "--uri" && i + 1 < argc) {
      uri = argv[++i];
    } else if (arg == "--rate" && i + 1 < argc) {
      rate_hz = std::stod(argv[++i]);
    } else if (arg == "--affinity" && i + 1 < argc) {
      affinityStr = argv[++i];                  // comma separated
    } else if (arg == "--prio" && i + 1 < argc) {
      std::string p = argv[++i];
      if (p == "realtime") realtime = true;
    } else if (arg == "--spin-tail") {
      spinTail = true;
    } else if (arg == "--csv" && i + 1 < argc) {
      csvPath = argv[++i];
    }
  }

#ifdef _WIN32
  SetConsoleCtrlHandler(consoleHandler, TRUE);
#endif

  // --- RT setup -----------------------------------------------------------
  unsigned long long mask = 0ull;
  if (!affinityStr.empty()) {
    std::stringstream ss(affinityStr);
    std::string token;
    while (std::getline(ss, token, ',')) {
      if (!token.empty()) {
        int core = std::stoi(token);
        if (core >= 0 && core < 64) mask |= (1ull << core);
      }
    }
  }

  RtConfig cfg;
  cfg.realtime    = realtime;
  cfg.affinityMask= mask;
  cfg.spinTail    = spinTail;
  init_timing(cfg);

  // --- IO setup -----------------------------------------------------------
  PwmState pwm;
  UdpInput udp;
  udp.start(&pwm);

  RadioClient radio(uri);
  try {
    radio.connect();
  } catch (const std::exception& e) {
    std::cerr << "[radio] init failed: " << e.what() << "\n";
    restore_timing();
    return 1;
  }

  // --- timing -------------------------------------------------------------
  const std::int64_t fq = qpc_freq();
  Metrics metrics(fq, csvPath);

  const double period_s = 1.0 / std::max(1.0, rate_hz);
  const std::int64_t period_ticks = secondsToTicks(period_s, fq);
  std::int64_t next = qpc_now();

  // --- loop ---------------------------------------------------------------
  while (g_running.load(std::memory_order_relaxed)) {
    std::uint16_t m1, m2, m3, m4;
    unpack4u16(pwm.packed.load(std::memory_order_acquire), m1, m2, m3, m4);
    radio.send4pwm(m1, m2, m3, m4);

    next += period_ticks;
    const std::int64_t now = qpc_now();
    const bool late = now > next;
    metrics.update(now, period_ticks, late);

    // 粗睡 + 尾端自旋（可選）
    const double slack_ms = ticksToMs(next - now, fq);
    if (slack_ms > 0.6) {
#ifdef _WIN32
      ::Sleep(static_cast<DWORD>(std::max(0.0, slack_ms - 0.3)));
#else
      Sleep(static_cast<unsigned long>(std::max(0.0, slack_ms - 0.3)));
#endif
    }
    if (cfg.spinTail) {
      while (qpc_now() < next) { /* spin */ }
    }

    metrics.maybePrint(now);
  }

  // --- graceful stop: 送幾次 0 PWM ---------------------------------------
  for (int i = 0; i < 10; ++i) {
    radio.send4pwm(0, 0, 0, 0);
    Sleep(10);
  }

  // --- teardown -----------------------------------------------------------
  radio.disconnect();
  udp.stop();

  const std::int64_t endTicks = qpc_now();
  metrics.finish(endTicks);
  restore_timing();

  return 0;
}
