#include <Windows.h>
#include <mmsystem.h>

#include "cf4pwm/timing_win.hpp"

#ifdef _WIN32
#include <windows.h>
#endif

namespace cf4pwm {
namespace {
bool g_timePeriod = false;
}

void init_timing(const RtConfig& cfg) {
#ifdef _WIN32
  timeBeginPeriod(1);
  g_timePeriod = true;
  HANDLE proc = GetCurrentProcess();
  SetPriorityClass(proc, cfg.realtime ? REALTIME_PRIORITY_CLASS : HIGH_PRIORITY_CLASS);
  HANDLE th = GetCurrentThread();
  SetThreadPriority(th, THREAD_PRIORITY_TIME_CRITICAL);
  if (cfg.affinityMask) {
    SetProcessAffinityMask(proc, static_cast<DWORD_PTR>(cfg.affinityMask));
    SetThreadAffinityMask(th, static_cast<DWORD_PTR>(cfg.affinityMask));
  }
#else
  (void)cfg;
#endif
}

void restore_timing() {
#ifdef _WIN32
  if (g_timePeriod) {
    timeEndPeriod(1);
    g_timePeriod = false;
  }
#endif
}

int64_t qpc_now() {
#ifdef _WIN32
  LARGE_INTEGER v;
  QueryPerformanceCounter(&v);
  return v.QuadPart;
#else
  return 0;
#endif
}

int64_t qpc_freq() {
#ifdef _WIN32
  static int64_t f = []() {
    LARGE_INTEGER v;
    QueryPerformanceFrequency(&v);
    return v.QuadPart;
  }();
  return f;
#else
  return 1;
#endif
}

int64_t secondsToTicks(double s, int64_t fq) {
  return static_cast<int64_t>(s * static_cast<double>(fq));
}

double ticksToMs(int64_t t, int64_t fq) {
  return static_cast<double>(t) * 1000.0 / static_cast<double>(fq);
}

int64_t ticksToNs(int64_t t, int64_t fq) {
  return static_cast<int64_t>(static_cast<double>(t) * 1e9 / static_cast<double>(fq));
}

} // namespace cf4pwm

