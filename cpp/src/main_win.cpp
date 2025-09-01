// main_win.cpp (UTF-8, no BOM)

#pragma pack(push, 1)
#include <crazyflie_cpp/Crazyflie.h>
#include <crazyflie_cpp/crtp.h>
#pragma pack(pop)

#include "cf4pwm/pack.hpp"
#include "cf4pwm/metrics.hpp"
#include "cf4pwm/udp_input.hpp"
#include "cf4pwm/radio_cfclient.hpp"
#include "cf4pwm/timing_win.hpp"
#include "cf4pwm/xyz_stream.hpp"

#include <atomic>
#include <cstdint>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>

#ifdef _WIN32
  #include <Windows.h>
#else
  #include <unistd.h>
  static inline void Sleep(unsigned long ms) { usleep(ms * 1000); }
#endif

using namespace std::chrono_literals;

int main(int argc, char** argv) {
  using cf4pwm::Metrics;
  using cf4pwm::RtConfig;
  using cf4pwm::UdpInput;
  using cf4pwm::PwmState;
  using cf4pwm::RadioClient;

  // ---- Ctrl+C handling ---------------------------------------------------
  static std::atomic<bool> g_running{true};
  static std::atomic<int>  g_ctrlCount{0};
  static std::atomic<bool> g_requestLanding{false};
  static std::atomic<bool> g_hasLanded{false};
#ifdef _WIN32
  auto consoleHandler = +[](DWORD ctrlType) -> BOOL {
    // Treat all console signals as a request to land, do not exit immediately
    (void)ctrlType;
    g_ctrlCount.fetch_add(1, std::memory_order_relaxed);
    g_requestLanding.store(true, std::memory_order_release);
    g_running.store(false, std::memory_order_release);
    // Block here until landing completes or timeout to avoid abrupt termination
    // Allow up to 15 seconds for landing sequence to finish
    const DWORD deadlineMs = 15000;
    DWORD waited = 0;
    while (!g_hasLanded.load(std::memory_order_acquire) && waited < deadlineMs) {
      ::Sleep(50);
      waited += 50;
    }
    return TRUE; // handled
  };
  SetConsoleCtrlHandler((PHANDLER_ROUTINE)consoleHandler, TRUE);
#endif

  // --- CLI options --------------------------------------------------------
  std::string uri = "radio://0/80/2M";
  double rate_hz = 500.0;
  std::string affinityStr;   // e.g. "3" or "3,5"
  bool realtime = false;     // --prio realtime
  bool spinTail = false;     // --spin-tail
  const char* csvPath = nullptr; // --csv <file>
  // UDP options
  int udpPort = 8888;
  bool udpDebug = false;
  cf4pwm::UdpInput::Expect udpExpect = cf4pwm::UdpInput::Expect::Auto;
  unsigned udpTimeoutMs = 100; // --udp-timeout-ms (optional)
  double statusRateHz = 5.0;   // --status-rate-hz
  int radioAckFailN = 5;       // --radio-ack-fail-N (hidden)
  // 4PWM enable/disable options
  std::string enableParamOverride; // --enable-param <name>
  bool autoDisable = true;         // --no-auto-disable -> false
  // XYZ target streaming (all values in millimeters)
  cf4pwm::XyzTarget xyzTarget; // stores millimeters
  double xyzRateHz = 200.0;    // default stream rate
  std::string xyzHost = "127.0.0.1";
  int xyzPort = 51002;
  // Keep user-facing XYZ in millimeters
  double x0_mm = 0.0, y0_mm = 0.0, z0_mm = 0.0;
  // S-curve flight sequence (millimeters)
  double takeoffZ_mm = 700.0;
  bool   takeoffZ_overridden = false; // track if user provided --takeoff-z
  double takeoffT = 4.0;
  double cruiseT  = 120.0;
  double landT    = 4.0;

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
    } else if (arg == "--udp-port" && i + 1 < argc) {
      udpPort = std::stoi(argv[++i]);
      if (udpPort < 1 || udpPort > 65535) udpPort = 8888;
    } else if (arg == "--udp-debug") {
      udpDebug = true;
    } else if (arg == "--udp-expect" && i + 1 < argc) {
      std::string v = argv[++i];
      if (v == "auto") udpExpect = cf4pwm::UdpInput::Expect::Auto;
      else if (v == "float") udpExpect = cf4pwm::UdpInput::Expect::Float;
      else if (v == "u16") udpExpect = cf4pwm::UdpInput::Expect::U16;
    } else if (arg == "--udp-timeout-ms" && i + 1 < argc) {
      int v = std::stoi(argv[++i]);
      if (v < 0) v = 0; // 0 disables watchdog engagement until first packet
      udpTimeoutMs = static_cast<unsigned>(v);
    } else if (arg == "--status-rate-hz" && i + 1 < argc) {
      statusRateHz = std::stod(argv[++i]);
      if (statusRateHz <= 0.0) statusRateHz = 5.0;
    } else if (arg == "--radio-ack-fail-N" && i + 1 < argc) {
      radioAckFailN = std::stoi(argv[++i]);
      if (radioAckFailN < 1) radioAckFailN = 5;
    } else if (arg == "--enable-param" && i + 1 < argc) {
      enableParamOverride = argv[++i];
    } else if (arg == "--no-auto-disable") {
      autoDisable = false;
    } else if (arg == "--target" && i + 1 < argc) {
      std::string s = argv[++i];
      // parse x,y,z (millimeters)
      double vals[3] = {0,0,0};
      int idx = 0;
      std::string cur;
      for (size_t k=0; k<=s.size(); ++k) {
        if (k==s.size() || s[k]==',') {
          if (!cur.empty() && idx < 3) {
            try { vals[idx] = std::stod(cur); } catch (...) {}
          }
          cur.clear();
          ++idx; if (idx>=3) break;
        } else {
          cur.push_back(s[k]);
        }
      }
      x0_mm = vals[0]; y0_mm = vals[1]; z0_mm = vals[2];
    } else if (arg == "--xyz-rate" && i + 1 < argc) {
      xyzRateHz = std::stod(argv[++i]);
    } else if (arg == "--xyz" && i + 1 < argc) {
      std::string s = argv[++i];
      double vals[3] = {0,0,0};
      int idx = 0; std::string cur;
      for (size_t k=0; k<=s.size(); ++k) {
        if (k==s.size() || s[k]==',') {
          if (!cur.empty() && idx < 3) { try { vals[idx] = std::stod(cur); } catch (...) {} }
          cur.clear();
          ++idx; if (idx>=3) break;
        } else { cur.push_back(s[k]); }
      }
      x0_mm = vals[0]; y0_mm = vals[1]; z0_mm = vals[2];
    } else if (arg == "--xyz-host" && i + 1 < argc) {
      xyzHost = argv[++i];
    } else if (arg == "--xyz-port" && i + 1 < argc) {
      xyzPort = std::stoi(argv[++i]);
      if (xyzPort < 1 || xyzPort > 65535) xyzPort = 51002;
    } else if (arg == "--takeoff-z" && i + 1 < argc) {
      // millimeters
      takeoffZ_mm = std::stod(argv[++i]);
      takeoffZ_overridden = true;
    } else if (arg == "--takeoff-T" && i + 1 < argc) {
      takeoffT = std::stod(argv[++i]);
      if (takeoffT < 0.0) takeoffT = 0.0;
    } else if (arg == "--land-T" && i + 1 < argc) {
      landT = std::stod(argv[++i]);
      if (landT < 0.0) landT = 0.0;
    } else if (arg == "--cruise" && i + 1 < argc) {
      cruiseT = std::stod(argv[++i]);
      if (cruiseT < 0.0) cruiseT = 0.0;
    }
  }

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
  cfg.realtime     = realtime;
  cfg.affinityMask = mask;
  cfg.spinTail     = spinTail;
  cf4pwm::init_timing(cfg);

  // --- IO setup -----------------------------------------------------------
  PwmState pwm;
  UdpInput udp;
  udp.configure(static_cast<uint16_t>(udpPort), udpDebug, udpExpect);
  udp.start(&pwm);   // 非阻塞收 8888 -> pwm.packed (atomic<uint64_t>)

  RadioClient radio(uri);
  try {
    radio.connect();
  } catch (const std::exception& e) {
    std::cerr << "[radio] init failed: " << e.what() << "\n";
    cf4pwm::restore_timing();
    return 1;
  }

  // Open Crazyflie link for param enable (fast path still uses Crazyradio directly)
  std::string enabledParamName; // if non-empty, indicates which param was set
  try {
    Crazyflie cf(uri);
    auto tryEnable = [&](const std::string& fullName) -> bool {
      auto dot = fullName.find('.');
      if (dot == std::string::npos || dot == 0 || dot + 1 >= fullName.size()) return false;
      std::string group = fullName.substr(0, dot);
      std::string name  = fullName.substr(dot + 1);
      try {
        uint8_t one = 1;
        cf.setParamByName(group.c_str(), name.c_str(), one);
        return true;
      } catch (const std::exception&) {
        return false;
      }
    };

    bool enabled = false;
    if (!enableParamOverride.empty()) {
      enabled = tryEnable(enableParamOverride);
      if (enabled) enabledParamName = enableParamOverride;
    } else {
      const char* candidates[] = { "crtp_pwm.enable", "pwm.enable", "motorPowerSet.enable" };
      for (const char* cand : candidates) {
        if (tryEnable(cand)) { enabledParamName = cand; enabled = true; break; }
      }
    }
    if (!enabled) {
      std::cerr << "[warn] Could not enable 4PWM via parameter; motors may not spin\n";
    }
  } catch (const std::exception& e) {
    std::cerr << "[warn] Crazyflie link/param setup failed: " << e.what() << "\n";
  }

  // If user set z via --xyz/--target and did not set --takeoff-z, use it as final takeoff height
  if (!takeoffZ_overridden && z0_mm > 0.0) {
    takeoffZ_mm = z0_mm;
  }

  // Consolidated connection message with enable parameter info
  std::cerr << "[radio] connected (ch=" << radio.channel() << ", rate=" << radio.rate()
            << ") | enable=" << (enabledParamName.empty() ? "-" : enabledParamName) << "\n";

  // Initialize atomics with starting target in millimeters
  // Always start from 0 mm on Z and ramp up in takeoff
  xyzTarget.x.store(static_cast<float>(x0_mm), std::memory_order_relaxed);
  xyzTarget.y.store(static_cast<float>(y0_mm), std::memory_order_relaxed);
  xyzTarget.z.store(0.0f, std::memory_order_relaxed);

  // Start XYZ UDP stream to MATLAB/Simulink
  double xyzRate = (xyzRateHz > 0.0) ? xyzRateHz : 200.0;
  cf4pwm::start_xyz_stream(&xyzTarget, xyzHost.c_str(), static_cast<uint16_t>(xyzPort), xyzRate);

  // --- timing/metrics -----------------------------------------------------
  const std::int64_t fq = cf4pwm::qpc_freq();
  Metrics metrics(fq, csvPath, statusRateHz);

  const double period_s = 1.0 / std::max(1.0, rate_hz);
  const std::int64_t period_ticks = cf4pwm::secondsToTicks(period_s, fq);
  std::int64_t next = cf4pwm::qpc_now();

  // --- flight sequence state ---------------------------------------------
  auto sCurve01 = [](double t, double T) -> double {
    if (T <= 0.0) return 1.0;
    if (t <= 0.0) return 0.0;
    if (t >= T) return 1.0;
    return 0.5 * (1.0 - std::cos(M_PI * t / T));
  };

  enum class Phase { Takeoff, Cruise, Landing, Done };
  Phase phase = Phase::Takeoff;
  const double fq_d = static_cast<double>(fq);
  std::int64_t phaseStart = cf4pwm::qpc_now();
  // keep millimeters internally for phase math
  // Always ramp from 0 mm on takeoff
  double z0 = 0.0;
  double zLandingStart = 0.0;

  // --- loop ---------------------------------------------------------------
  // small inline clamp helper (no overhead)
  auto clamp_u16 = [](uint32_t v) -> uint16_t {
    return static_cast<uint16_t>(v > 65535u ? 65535u : v);
  };

  uint64_t loopCounter = 0;
  bool udpWatchdogTripped = false;
  bool radioWatchdogTripped = false;

  while (phase != Phase::Done) {
    std::uint16_t m1, m2, m3, m4;
    const std::uint64_t packed = pwm.packed.load(std::memory_order_acquire);

    // 使用我們的 pack/unpack 介面（命名空間限定，避免撞名）
    cf4pwm::unpack4u16(packed, m1, m2, m3, m4);
    // Clamp defensively before sending to radio
    m1 = clamp_u16(m1); m2 = clamp_u16(m2); m3 = clamp_u16(m3); m4 = clamp_u16(m4);
    const bool ackOk = radio.send4pwm(m1, m2, m3, m4);
    metrics.onAck(ackOk);

    next += period_ticks;
    const std::int64_t now = cf4pwm::qpc_now();
    const bool late = now > next;
    metrics.update(now, period_ticks, late);

    // UDP last receive watchdog (check every ~10 loops to reduce overhead)
    if (((++loopCounter) % 10ull) == 0ull && !udpWatchdogTripped) {
      const int64_t lastRx = udp.last_rx_ticks.load(std::memory_order_acquire);
      if (lastRx != 0 && udpTimeoutMs > 0) {
        const int64_t deltaTicks = now - lastRx;
        const double deltaMs = cf4pwm::ticksToMs(deltaTicks, fq);
        if (deltaMs > static_cast<double>(udpTimeoutMs)) {
          // Engage failsafe: one immediate zero PWM, then exit
          std::cerr << "\n[FAILSAFE] UDP timeout: no packet > " << udpTimeoutMs << " ms. Stopping.\n";
          radio.send4pwm(0, 0, 0, 0);
          g_running.store(false, std::memory_order_release);
          udpWatchdogTripped = true;
          phase = Phase::Done;
          break;
        }
      }
    }

    // Radio ACK consecutive failure watchdog
    if (!radioWatchdogTripped) {
      const int streak = metrics.ackFailStreak();
      if (streak >= radioAckFailN) {
        std::cerr << "\n[FAILSAFE] Radio ACK failures x" << streak << " (>= " << radioAckFailN << "). Stopping.\n";
        radio.send4pwm(0, 0, 0, 0);
        g_running.store(false, std::memory_order_release);
        radioWatchdogTripped = true;
        phase = Phase::Done;
        break;
      }
    }

    const double tPhase = static_cast<double>(now - phaseStart) / fq_d;

    if (g_requestLanding.load(std::memory_order_relaxed)) {
      if (phase != Phase::Landing && phase != Phase::Done) {
        // capture current mm for landing start
        zLandingStart = static_cast<double>(xyzTarget.z.load(std::memory_order_relaxed));
        std::cerr << "[signal] Ctrl+C received -> initiating landing...\n";
        phase = Phase::Landing;
        phaseStart = now;
      }
    }

    switch (phase) {
      case Phase::Takeoff: {
        double z = z0 + (takeoffZ_mm - z0) * sCurve01(tPhase, takeoffT); // mm
        xyzTarget.z.store(static_cast<float>(z), std::memory_order_relaxed);
        if (tPhase >= takeoffT) {
          phase = Phase::Cruise;
          phaseStart = now;
        }
        break;
      }
      case Phase::Cruise: {
        xyzTarget.z.store(static_cast<float>(takeoffZ_mm), std::memory_order_relaxed);
        if (tPhase >= cruiseT) {
          zLandingStart = takeoffZ_mm;
          phase = Phase::Landing;
          phaseStart = now;
        }
        break;
      }
      case Phase::Landing: {
        double z = zLandingStart * (1.0 - sCurve01(tPhase, landT)); // mm
        if (z < 0.0) z = 0.0;
        xyzTarget.z.store(static_cast<float>(z), std::memory_order_relaxed);
        if (tPhase >= landT) {
          xyzTarget.z.store(0.0f, std::memory_order_relaxed);
          phase = Phase::Done;
          g_hasLanded.store(true, std::memory_order_release);
        }
        break;
      }
      case Phase::Done:
        break;
    }

    // 粗睡 + 尾端自旋（可選）
    const double slack_ms = cf4pwm::ticksToMs(next - now, fq);
    if (slack_ms > 0.6) {
      Sleep(static_cast<unsigned long>(std::max(0.0, slack_ms - 0.3)));
    }
    if (cfg.spinTail) {
      while (cf4pwm::qpc_now() < next) { /* spin */ }
    }

    if (metrics.maybePrintBegin(now)) {
      // Snap to integers for display to avoid transient float prints
      int px = static_cast<int>(std::lround(xyzTarget.x.load(std::memory_order_relaxed)));
      int py = static_cast<int>(std::lround(xyzTarget.y.load(std::memory_order_relaxed)));
      int pz = static_cast<int>(std::lround(xyzTarget.z.load(std::memory_order_relaxed)));
      std::cout << " | m1~m4: [" << m1 << ", " << m2 << ", " << m3 << ", " << m4
                << "] | xyz_mm: [" << px << ", " << py << ", " << pz << "]" << std::endl;
    }
  }

  // --- graceful stop: 送幾次 0 PWM ---------------------------------------
  for (int i = 0; i < 10; ++i) {
    radio.send4pwm(0, 0, 0, 0);
    Sleep(10);
  }

  // --- teardown -----------------------------------------------------------
  // Disable 4PWM if we enabled it (unless opted out)
  if (autoDisable && !enabledParamName.empty()) {
    try {
      Crazyflie cf(uri);
      auto dot = enabledParamName.find('.');
      if (dot != std::string::npos && dot > 0 && dot + 1 < enabledParamName.size()) {
        std::string group = enabledParamName.substr(0, dot);
        std::string name  = enabledParamName.substr(dot + 1);
        uint8_t zero = 0;
        cf.setParamByName(group.c_str(), name.c_str(), zero);
      }
    } catch (const std::exception& e) {
      std::cerr << "[warn] Failed to disable 4PWM param: " << e.what() << "\n";
    }
  }

  radio.disconnect();
  udp.stop();
  cf4pwm::stop_xyz_stream();

  const std::int64_t endTicks = cf4pwm::qpc_now();
  metrics.finish(endTicks);
  cf4pwm::restore_timing();

  return 0;
}
