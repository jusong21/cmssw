/*
 * See IRPCSignal.h.
 *
 * \author J. Shin -- Kyung Hee University
 */

#include "IRPCSignal.h"

#include <cmath>

bool isAdjacentStrip(int stripA, int stripB, int maxStripJump) {
  return std::abs(stripA - stripB) <= 1 + maxStripJump;
}

bool isAdjacentTime(float timeA, float timeB, float thrTime) {
  return std::abs(timeA - timeB) < thrTime;
}

float stripNumAvg(const std::vector<IRPCSignal>& signals) {
  if (signals.empty()) {
    return 0.f;
  }
  long sumStrip = 0;
  for (const auto& s : signals) {
    sumStrip += s.strip;
  }
  return sumStrip / signals.size();
}
