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
      nHR_(0),
      sumHR_(0.f),
      sumHR2_(0.f),
      nLR_(0),
      sumLR_(0.f),
      sumLR2_(0.f),
      sumY_(0.f),
      sumY2_(0.f),
      nY_(0) {}

IRPCCluster::~IRPCCluster() = default;

int IRPCCluster::firstStrip() const { return fstrip_; }

int IRPCCluster::lastStrip() const { return lstrip_; }

int IRPCCluster::clusterSize() const { return lstrip_ - fstrip_ + 1; }

int IRPCCluster::bx() const { return bx_; }

bool IRPCCluster::hasHRTime() const { return nHR_ > 0; }

float IRPCCluster::hrTime() const { return hasHRTime() ? sumHR_ / nHR_ : 0.f; }

float IRPCCluster::hrTimeRMS() const { return rmsFromSums(sumHR_, sumHR2_, nHR_); }

bool IRPCCluster::hasLRTime() const { return nLR_ > 0; }

float IRPCCluster::lrTime() const { return hasLRTime() ? sumLR_ / nLR_ : 0.f; }

float IRPCCluster::lrTimeRMS() const { return rmsFromSums(sumLR_, sumLR2_, nLR_); }

bool IRPCCluster::hasTime() const { return nHR_ > 0 && nLR_ > 0; }

float IRPCCluster::time() const {
  if (nHR_ > 0 && nLR_ > 0) {
    return 0.5f * (sumHR_ / nHR_ + sumLR_ / nLR_);
  }
  return 0.f;
}

float IRPCCluster::timeRMS() const {
  if (nHR_ > 0 && nLR_ > 0) {
    const float sHR = rmsFromSums(sumHR_, sumHR2_, nHR_);
    const float sLR = rmsFromSums(sumLR_, sumLR2_, nLR_);
    if (sHR < 0.f || sLR < 0.f) {
      return -1.f;
    }
    return 0.5f * std::sqrt(sHR * sHR + sLR * sLR);
  }
  return -1.f;
}

bool IRPCCluster::hasY() const { return nY_ > 0; }

float IRPCCluster::y() const { return hasY() ? sumY_ / nY_ : 0.f; }

float IRPCCluster::yRMS() const {
  if (!hasY()) {
    return -1.f;
  }
  return std::sqrt(std::max(0.f, sumY2_ * nY_ - sumY_ * sumY_)) / nY_;
}

IRPCCluster IRPCCluster::compute(const std::vector<IRPCSignal>& signals, float speed) {
  IRPCCluster cl;
  if (signals.empty()) {
    return cl;
  }

  int fstrip = std::numeric_limits<int>::max();
  int lstrip = std::numeric_limits<int>::min();
  int bx = 0;

  uint16_t nHr = 0;
  float sumHr = 0.f;
  float sumHr2 = 0.f;
  uint16_t nLr = 0;
  float sumLr = 0.f;
  float sumLr2 = 0.f;

  for (std::size_t i = 0; i < signals.size(); ++i) {
    const IRPCSignal& s = signals[i];
    fstrip = std::min(fstrip, s.strip);
    lstrip = std::max(lstrip, s.strip);
    if (i == 0) {
      bx = s.bx;
    }
    if (s.hr) {
      ++nHr;
      sumHr += s.time;
      sumHr2 += s.time * s.time;
    }
    if (s.lr) {
      ++nLr;
      sumLr += s.time;
      sumLr2 += s.time * s.time;
    }
  }

  uint16_t nY = 0;
  float sumY = 0.f;
  float sumY2 = 0.f;
  if (nHr > 0 && nLr > 0) {
    const float meanHR = sumHr / nHr;
    const float meanLR = sumLr / nLr;
    const float y = 0.5f * (meanLR - meanHR) * speed;
    nY = 1;
    sumY = y;
    sumY2 = y * y;
  }

  cl.fstrip_ = static_cast<uint16_t>(fstrip);
  cl.lstrip_ = static_cast<uint16_t>(lstrip);
  cl.bx_ = static_cast<int16_t>(bx);
  cl.nHR_ = nHr;
  cl.sumHR_ = sumHr;
  cl.sumHR2_ = sumHr2;
  cl.nLR_ = nLr;
  cl.sumLR_ = sumLr;
  cl.sumLR2_ = sumLr2;
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
