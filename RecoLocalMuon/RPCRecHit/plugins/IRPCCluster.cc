/*
 * See header file for a description of this class.
 *
 * \author J. Shin -- Kyung Hee University
 */

#include "IRPCCluster.h"

#include <algorithm>
#include <cmath>

namespace {
inline float rmsFromSums(float sum, float sum2, uint16_t n) {
  if (n == 0) {
    return -1.f;
  }
  return std::sqrt(std::max(0.f, sum2 * static_cast<float>(n) - sum * sum)) / static_cast<float>(n);
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
      nY_(0),
      sumY_(0.f),
      sumY2_(0.f) {}

IRPCCluster::~IRPCCluster() = default;

int IRPCCluster::firstStrip() const { return fstrip_; }

int IRPCCluster::lastStrip() const { return lstrip_; }

int IRPCCluster::clusterSize() const { return lstrip_ - fstrip_ + 1; }

int IRPCCluster::bx() const { return bx_; }

bool IRPCCluster::hasHighTime() const { return nHR_ > 0; }

float IRPCCluster::highTime() const { return hasHighTime() ? sumHR_ / static_cast<float>(nHR_) : 0.f; }

float IRPCCluster::highTimeRMS() const { return rmsFromSums(sumHR_, sumHR2_, nHR_); }

bool IRPCCluster::hasLowTime() const { return nLR_ > 0; }

float IRPCCluster::lowTime() const { return hasLowTime() ? sumLR_ / static_cast<float>(nLR_) : 0.f; }

float IRPCCluster::lowTimeRMS() const { return rmsFromSums(sumLR_, sumLR2_, nLR_); }

bool IRPCCluster::hasY() const { return nY_ > 0; }

float IRPCCluster::y() const { return hasY() ? sumY_ / static_cast<float>(nY_) : 0.f; }

float IRPCCluster::yRMS() const {
  if (!hasY()) {
    return -1.f;
  }
  return std::sqrt(std::max(0.f, sumY2_ * static_cast<float>(nY_) - sumY_ * sumY_)) / static_cast<float>(nY_);
}

void IRPCCluster::setClusterSummary(uint16_t fstrip,
                                    uint16_t lstrip,
                                    int16_t bx,
                                    uint16_t nHigh,
                                    float sumHigh,
                                    float sumHigh2,
                                    uint16_t nLow,
                                    float sumLow,
                                    float sumLow2,
                                    uint16_t nY,
                                    float sumY,
                                    float sumY2) {
  fstrip_ = fstrip;
  lstrip_ = lstrip;
  bx_ = bx;
  nHR_ = nHigh;
  sumHR_ = sumHigh;
  sumHR2_ = sumHigh2;
  nLR_ = nLow;
  sumLR_ = sumLow;
  sumLR2_ = sumLow2;
  nY_ = nY;
  sumY_ = sumY;
  sumY2_ = sumY2;
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
