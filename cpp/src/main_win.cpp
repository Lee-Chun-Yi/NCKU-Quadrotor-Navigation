#include "cf4pwm/metrics.hpp"
#include "cf4pwm/pack.hpp"
#include "cf4pwm/radio_cfclient.hpp"
#include "cf4pwm/timing_win.hpp"
#include "cf4pwm/udp_input.hpp"

#include <atomic>
#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>
#include <vector>
#include <chrono>

#ifdef _WIN32
#include <windows.h>
#endif

using namespace std::chrono_literals;
using namespace cf4pwm;

#ifndef _WIN32
static inline void Sleep(unsigned long) {}
#endif

static std::atomic<bool> g_running{true};

#ifdef _WIN32
BOOL WINAPI consoleHandler(DWORD) {
  g_running = false;
  return TRUE;
}
#endif

int main(int argc, char** argv) {
  std::string uri = "radio://0/80/2M";
  double rate = 500.0;
  std::string affinityStr;
  bool realtime = false;
  bool spinTail = false;
  const char* csvPath = nullptr;

  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    if (arg == "--uri" && i + 1 < argc) {
      uri = argv[++i];
    } else if (arg == "--rate" && i + 1 < argc) {
      rate = std::stod(argv[++i]);
    } else if (arg == "--affinity" && i + 1 < argc) {
      affinityStr = argv[++i];
    } else if (arg == "--prio" && i + 1 < argc) {
      std::string p = argv[++i];
      if (p == "realtime")
        realtime = true;
    } else if (arg == "--spin-tail") {
      spinTail = true;
    } else if (arg == "--csv" && i + 1 < argc) {
      csvPath = argv[++i];
    }
  }

#ifdef _WIN32
  SetConsoleCtrlHandler(consoleHandler, TRUE);
#endif

  unsigned long long mask = 0;
  if (!affinityStr.empty()) {
    std::stringstream ss(affinityStr);
    std::string token;
    while (std::getline(ss, token, ',')) {
      int core = std::stoi(token);
      mask |= (1ull << core);
    }
  }

  RtConfig cfg;
  cfg.realtime = realtime;
  cfg.affinityMask = mask;
  cfg.spinTail = spinTail;
  init_timing(cfg);

  PwmState pwm;
  UdpInput udp;
  udp.start(&pwm);

  RadioClient radio;
  if (!radio.init(uri)) {
    std::cerr << "radio init failed\n";
    return 1;
  }

  int64_t fq = qpc_freq();
  Metrics metrics(fq, csvPath);
  double period_s = 1.0 / rate;
  int64_t period_ticks = secondsToTicks(period_s, fq);
  int64_t next = qpc_now();

  while (g_running) {
    uint16_t m1, m2, m3, m4;
    unpack4u16(pwm.packed.load(std::memory_order_acquire), m1, m2, m3, m4);
    radio.send4(m1, m2, m3, m4);

    next += period_ticks;
    int64_t now = qpc_now();
    bool late = now > next;
    metrics.update(now, period_ticks, late);

    double slack_ms = ticksToMs(next - now, fq);
    if (slack_ms > 0.6)
      Sleep((DWORD)std::max(0.0, slack_ms - 0.3));
    if (cfg.spinTail) {
      while (qpc_now() < next) {
      }
    }
    metrics.maybePrint(now);
  }

  for (int i = 0; i < 10; ++i) {
    radio.send4(0, 0, 0, 0);
    Sleep(10);
  }

  radio.close();
  udp.stop();
  int64_t endTicks = qpc_now();
  metrics.finish(endTicks);
  restore_timing();
  return 0;
}

