#include "cf4pwm/metrics.hpp"
#include "cf4pwm/timing_win.hpp"

#include <algorithm>
#include <cstdlib>
#include <iomanip>
#include <iostream>

namespace cf4pwm {

Metrics::Metrics(int64_t freq, const char* csvPath)
    : freq_(freq), ring_(2048) {
  if (csvPath) {
    csv_.open(csvPath);
    if (csv_) {
      csv_ << "ts_ns,period_ns,jitter_ns,late\n";
    }
  }
}

void Metrics::update(int64_t nowTicks, int64_t periodTicks, bool late) {
  if (startTicks_ == 0) {
    startTicks_ = nowTicks;
    lastTicks_ = nowTicks;
    lastPrintTicks_ = nowTicks;
    return;
  }

  int64_t actualTicks = nowTicks - lastTicks_;
  lastTicks_ = nowTicks;
  int64_t jitterTicks = actualTicks - periodTicks;
  int64_t jitterNs = ticksToNs(jitterTicks, freq_);

  ring_[ringPos_++] = std::llabs(jitterNs);
  if (ringPos_ >= ring_.size())
    ringPos_ = 0;

  if (jitterNs < minJitter_)
    minJitter_ = jitterNs;
  if (jitterNs > maxJitter_)
    maxJitter_ = jitterNs;

  ++samples_;
  if (late)
    ++late_;

  if (csv_) {
    int64_t ts = ticksToNs(nowTicks - startTicks_, freq_);
    int64_t periodNs = ticksToNs(actualTicks, freq_);
    csv_ << ts << ',' << periodNs << ',' << jitterNs << ',' << (late ? 1 : 0)
         << '\n';
  }
}

void Metrics::maybePrint(int64_t nowTicks) {
  int64_t elapsedTicks = nowTicks - lastPrintTicks_;
  if (elapsedTicks < freq_)
    return; // less than a second

  uint64_t diffSamples = samples_ - samplesAtPrint_;
  uint64_t diffLate = late_ - lateAtPrint_;
  double hz = diffSamples * static_cast<double>(freq_) / elapsedTicks;
  double latePct = diffSamples ? (100.0 * diffLate / diffSamples) : 0.0;

  std::vector<int64_t> data = ring_;
  std::sort(data.begin(), data.end());
  auto pct = [&](double p) -> int64_t {
    size_t idx = static_cast<size_t>(p * (data.size() - 1));
    return data[idx];
  };
  int64_t p50 = pct(0.50);
  int64_t p90 = pct(0.90);
  int64_t p99 = pct(0.99);

  std::cout << std::fixed << std::setprecision(1)
            << "rate=" << hz << " Hz | late=" << latePct
            << "% | jitter_us p50=" << p50 / 1000
            << " p90=" << p90 / 1000
            << " p99=" << p99 / 1000
            << " max=" << maxJitter_ / 1000 << std::endl;

  lastPrintTicks_ = nowTicks;
  samplesAtPrint_ = samples_;
  lateAtPrint_ = late_;
  minJitter_ = INT64_MAX;
  maxJitter_ = INT64_MIN;
}

bool Metrics::maybePrintBegin(int64_t nowTicks) {
  int64_t elapsedTicks = nowTicks - lastPrintTicks_;
  if (elapsedTicks < freq_)
    return false; // less than a second

  uint64_t diffSamples = samples_ - samplesAtPrint_;
  uint64_t diffLate = late_ - lateAtPrint_;
  double hz = diffSamples * static_cast<double>(freq_) / elapsedTicks;
  double latePct = diffSamples ? (100.0 * diffLate / diffSamples) : 0.0;

  std::vector<int64_t> data = ring_;
  std::sort(data.begin(), data.end());
  auto pct = [&](double p) -> int64_t {
    size_t idx = static_cast<size_t>(p * (data.size() - 1));
    return data[idx];
  };
  int64_t p50 = pct(0.50);
  int64_t p90 = pct(0.90);
  int64_t p99 = pct(0.99);

  std::cout << std::fixed << std::setprecision(1)
            << "rate=" << hz << " Hz | late=" << latePct
            << "% | jitter_us p50=" << p50 / 1000
            << " p90=" << p90 / 1000
            << " p99=" << p99 / 1000
            << " max=" << maxJitter_ / 1000;

  lastPrintTicks_ = nowTicks;
  samplesAtPrint_ = samples_;
  lateAtPrint_ = late_;
  minJitter_ = INT64_MAX;
  maxJitter_ = INT64_MIN;
  return true;
}

void Metrics::finish(int64_t nowTicks) {
  maybePrint(nowTicks);
  if (csv_)
    csv_.close();
}

} // namespace cf4pwm
