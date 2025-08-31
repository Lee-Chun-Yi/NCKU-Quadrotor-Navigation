#pragma once

#include <cstdint>
#include <fstream>
#include <vector>

namespace cf4pwm {

class Metrics {
public:
  Metrics(int64_t freq, const char* csvPath = nullptr);
  void update(int64_t nowTicks, int64_t periodTicks, bool late);
  bool maybePrintBegin(int64_t nowTicks); // prints base line without newline; returns true if printed
  void maybePrint(int64_t nowTicks);
  void finish(int64_t nowTicks);

private:
  int64_t freq_;
  std::vector<int64_t> ring_;
  size_t ringPos_ = 0;
  uint64_t samples_ = 0;
  uint64_t late_ = 0;
  int64_t lastTicks_ = 0;
  int64_t lastPrintTicks_ = 0;
  uint64_t samplesAtPrint_ = 0;
  uint64_t lateAtPrint_ = 0;
  int64_t minJitter_ = INT64_MAX;
  int64_t maxJitter_ = INT64_MIN;
  std::ofstream csv_;
  int64_t startTicks_ = 0;
};

} // namespace cf4pwm
