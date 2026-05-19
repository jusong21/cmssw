/*
 * See IRPCCluster.h.
 *
 * \author J. Shin -- Kyung Hee University
 */

#include "IRPCCluster.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace {
inline float rmsFromSums(float sum, float sum2, uint16_t n) {
  if (n == 0) {
    return -1.f;
  }
  return std::sqrt(std::max(0.f, sum2 * n - sum * sum)) / n;
}
}  // namespace

IRPCCluster::IRPCCluster()
    : fstrip_(0),
      lstrip_(0),
      bx_(0),
      sumTime_(0.f),
      sumTime2_(0.f),
      nTime_(0),
      sumY_(0.f),
      sumY2_(0.f),
      nY_(0) {}

IRPCCluster::~IRPCCluster() = default;

int IRPCCluster::firstStrip() const { return fstrip_; }

int IRPCCluster::lastStrip() const { return lstrip_; }

int IRPCCluster::clusterSize() const { return lstrip_ - fstrip_ + 1; }

int IRPCCluster::bx() const { return bx_; }

bool IRPCCluster::hasTime() const { return nTime_ > 0; }

float IRPCCluster::time() const { return hasTime() ? sumTime_ / nTime_ : 0.f; }

float IRPCCluster::timeRMS() const { return hasTime() ? rmsFromSums(sumTime_, sumTime2_, nTime_) : -1.f; }

bool IRPCCluster::hasY() const { return nY_ > 0; }

float IRPCCluster::y() const { return hasY() ? sumY_ / nY_ : 0.f; }

float IRPCCluster::yRMS() const {
  if (!hasY()) {
    return -1.f;
  }
  return std::sqrt(std::max(0.f, sumY2_ * nY_ - sumY_ * sumY_)) / nY_;
}

IRPCCluster IRPCCluster::compute(const std::vector<IRPCSignal>& signals) {
  IRPCCluster cl;
  if (signals.empty()) {
    return cl;
  }

  int fstrip = std::numeric_limits<int>::max();
  int lstrip = std::numeric_limits<int>::min();
  int bx = 0;

  // Use the HR signal from each digi (one per digi) to average digiTime and digiY.
  // digiTime = 0.5*(t_HR + t_LR) and digiY = 0.5*(t_LR - t_HR)*speed were computed
  // per digi and stored in every IRPCSignal. Using HR signals avoids double-counting
  // since each digi contributes exactly one HR and one LR signal to the merged cluster.
  uint16_t nTime = 0, nY = 0;
  float sumTime = 0.f, sumTime2 = 0.f;
  float sumY = 0.f, sumY2 = 0.f;

  for (std::size_t i = 0; i < signals.size(); ++i) {
    const IRPCSignal& s = signals[i];
    fstrip = std::min(fstrip, s.strip);
    lstrip = std::max(lstrip, s.strip);
    if (i == 0) {
      bx = s.bx;
    }
    if (s.isHR) {
      ++nTime; sumTime += s.time; sumTime2 += s.time * s.time;
      ++nY;    sumY   += s.y;    sumY2   += s.y   * s.y;
    }
  }

  cl.fstrip_ = static_cast<uint16_t>(fstrip);
  cl.lstrip_ = static_cast<uint16_t>(lstrip);
  cl.bx_ = static_cast<int16_t>(bx);
  cl.nTime_ = nTime;
  cl.sumTime_ = sumTime;
  cl.sumTime2_ = sumTime2;
  cl.nY_ = nY;
  cl.sumY_ = sumY;
  cl.sumY2_ = sumY2;

  return cl;
}

bool IRPCCluster::operator<(const IRPCCluster& other) const {
  if (other.bx() == this->bx()) {
    return other.firstStrip() < this->firstStrip();
  }
  return other.bx() < this->bx();
}

bool IRPCCluster::operator==(const IRPCCluster& other) const {
  return (this->clusterSize() == other.clusterSize()) && (this->bx() == other.bx()) &&
         (this->firstStrip() == other.firstStrip());
}
