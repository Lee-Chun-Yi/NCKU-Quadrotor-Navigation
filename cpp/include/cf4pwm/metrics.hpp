#pragma once

#include <cstdint>
#include <fstream>
#include <vector>

namespace cf4pwm {

class Metrics {
public:
  Metrics(int64_t freq, const char* csvPath = nullptr, double statusRateHz = 5.0);
  void update(int64_t nowTicks, int64_t periodTicks, bool late);
  bool maybePrintBegin(int64_t nowTicks); // prints base line without newline; returns true if printed
  void maybePrint(int64_t nowTicks);
  void finish(int64_t nowTicks);
  
  // Record Crazyradio ACK result (true=ok, false=fail/timeout)
  void onAck(bool ok);
  
  // Current consecutive ACK failure streak (atomic read)
  int ackFailStreak() const { return ack_fail_streak_cur_.load(std::memory_order_acquire); }

private:
  int64_t freq_;
  int64_t printIntervalTicks_ = 0; // cadence for status line
  std::vector<int64_t> ring_;
  size_t ringPos_ = 0;
  uint64_t samples_ = 0;
  uint64_t late_ = 0;
  int64_t lastTicks_ = 0;
  int64_t lastPeriodTicks_ = 0;
  int64_t lastPrintTicks_ = 0;
  uint64_t samplesAtPrint_ = 0;
  uint64_t lateAtPrint_ = 0;
  int64_t minJitter_ = INT64_MAX;
  int64_t maxJitter_ = INT64_MIN;
  std::ofstream csv_;
  int64_t startTicks_ = 0;

  // Wireless ACK counters (atomics for safety)
  std::atomic<uint64_t> ack_ok_{0};
  std::atomic<uint64_t> ack_fail_{0};
  std::atomic<int>      ack_fail_streak_cur_{0};
  std::atomic<int>      ack_fail_streak_max_{0};
  // Snapshots used for windowed rates in status print
  uint64_t ack_ok_at_print_ = 0;
  uint64_t ack_fail_at_print_ = 0;
};

} // namespace cf4pwm
