/*
 * See header file for a description of this class.
 *
 * \author J. Shin -- Kyung Hee University
 */

#include "DataFormats/RPCDigi/interface/IRPCDigiTime.h"

#include "IRPCCluster.h"
#include "IRPCClusterizer.h"

#include <cstdint>
#include <algorithm>
#include <cmath>
#include <iterator>
#include <limits>
#include <map>
#include <vector>

namespace {

struct IRPCSignal {
  int strip = 0;
  float time = 0.f;
  int bx = 0;
  bool hr = false;
  bool lr = false;
};

struct IRPCOneSideCluster {
  std::vector<IRPCSignal> signals;
};

bool IRPCAdjacentStrip(int stripA, int stripB, int maxStripJump) {
  return std::abs(stripA - stripB) <= 1 + maxStripJump;
}

bool IRPCAdjacentTime(float timeA, float timeB, float thrTime) {
  return std::abs(timeA - timeB) < thrTime;
}

float IRPCStripNumAvg(const std::vector<IRPCSignal>& signals) {
  long sumStrip = 0;
  for (const auto& s : signals) {
    sumStrip += s.strip;
  }
  return static_cast<float>(sumStrip) / static_cast<float>(signals.size());
}

bool IRPCOneSideClusterizer(float thrTime,
                            std::vector<IRPCSignal>& signalCont,
                            std::vector<IRPCOneSideCluster>& clusters) {
  if (signalCont.empty()) {
    return false;
  }

  std::vector<std::pair<std::size_t, int>> signalFlags;
  signalFlags.reserve(signalCont.size());
  for (std::size_t i = 0; i < signalCont.size(); ++i) {
    signalFlags.emplace_back(i, 0);
  }

  // Find the first signal with the minimum time
  while (std::any_of(signalFlags.begin(), signalFlags.end(), [](const auto& sf) { return sf.second == 0; })) {
    float minTime = std::numeric_limits<float>::infinity();
    std::size_t minTimeIdx = signalFlags.size();
    // Iterate over all signals and find the one with the minimum time
    for (std::size_t i = 0; i < signalFlags.size(); ++i) {
      if (signalFlags[i].second == 0) {
        const auto& s = signalCont[signalFlags[i].first];
        if (s.time < minTime) {
          minTime = s.time;
          minTimeIdx = i;
        }
      }
    }
    if (minTimeIdx == signalFlags.size()) {
      break;
    }

    IRPCOneSideCluster tempCluster;
    tempCluster.signals.push_back(signalCont[signalFlags[minTimeIdx].first]); // Add the signal with the minimum time to the cluster
    signalFlags[minTimeIdx].second = -1; // Mark the signal as used

    const int maxStripJump = 1;
    std::size_t currentIdx = signalFlags[minTimeIdx].first; // Start from the signal with the minimum time
    bool nomatch = false;
    while (!nomatch) {
      if (currentIdx == 0) {
        break;
      }
      const std::size_t leftIdx = currentIdx - 1;
      IRPCSignal& leftSig = signalCont[leftIdx];
      IRPCSignal& refSig = signalCont[currentIdx];
      // Check if the left signal is adjacent to the reference signal and if the time difference is less than the threshold
      if (IRPCAdjacentStrip(leftSig.strip, refSig.strip, maxStripJump) &&
          IRPCAdjacentTime(leftSig.time, signalCont[signalFlags[minTimeIdx].first].time, thrTime)) {
        tempCluster.signals.push_back(leftSig);
        signalFlags[leftIdx].second = -1;
        --currentIdx;
      } else {
        nomatch = true;
        break;
      }
    }

    currentIdx = signalFlags[minTimeIdx].first;
    nomatch = false;
    while (!nomatch) {
      if (currentIdx + 1 >= signalCont.size()) {
        break;
      }
      const std::size_t rightIdx = currentIdx + 1;
      IRPCSignal& rightSig = signalCont[rightIdx];
      IRPCSignal& refSig = signalCont[currentIdx];
      // Check if the right signal is adjacent to the reference signal and if the time difference is less than the threshold
      if (IRPCAdjacentStrip(rightSig.strip, refSig.strip, maxStripJump) &&
          IRPCAdjacentTime(rightSig.time, signalCont[signalFlags[minTimeIdx].first].time, thrTime)) {
        tempCluster.signals.push_back(rightSig);
        signalFlags[rightIdx].second = -1;
        ++currentIdx;
      } else {
        nomatch = true;
        break;
      }
    }

    clusters.push_back(std::move(tempCluster));
  }
  return true;
}

std::vector<IRPCOneSideCluster> IRPCFinalClusterizer(const std::vector<IRPCOneSideCluster>& HR,
                                                     const std::vector<IRPCOneSideCluster>& LR,
                                                     float thrStripNum) {
  std::vector<IRPCOneSideCluster> out;
  std::vector<char> usedLR(LR.size(), 0);

  for (std::size_t iHR = 0; iHR < HR.size(); ++iHR) {
    const float stripHR = IRPCStripNumAvg(HR[iHR].signals);
    float minDeltaStr = std::numeric_limits<float>::infinity();
    std::size_t minDeltaStrILR = LR.size();

    for (std::size_t iLR = 0; iLR < LR.size(); ++iLR) {
      if (usedLR[iLR]) {
        continue;
      }
      const float stripLR = IRPCStripNumAvg(LR[iLR].signals);
      const float deltaStrip = std::abs(stripHR - stripLR);
      if (deltaStrip < minDeltaStr) {
        minDeltaStr = deltaStrip;
        minDeltaStrILR = iLR;
      }
    }

    // Strictly below threshold; each LR cluster used at most once (usedLR).
    if (minDeltaStrILR < LR.size() && minDeltaStr < thrStripNum) {
      IRPCOneSideCluster merged;
      merged.signals = HR[iHR].signals;
      merged.signals.insert(merged.signals.end(), LR[minDeltaStrILR].signals.begin(), LR[minDeltaStrILR].signals.end());
      out.push_back(std::move(merged));
      usedLR[minDeltaStrILR] = 1;
    }
  }

  return out;
}

IRPCClusterContainer IRPCClusteringDoAction(const IRPCDigiCollection::Range& digiRange,
                                            float thrTime,
                                            float thrStripNum,
                                            float speed) {
  IRPCClusterContainer out;

  if (std::distance(digiRange.first, digiRange.second) == 0) {
    return out;
  }

  std::map<int, std::pair<std::vector<IRPCSignal>, std::vector<IRPCSignal>>> signalsByBx;

  for (auto digi = digiRange.first; digi != digiRange.second; ++digi) {
    const int bunchX = digi->bx();
    const int strip = digi->strip();
    const float timeHR = IRPCDigiTime(*digi).timeHR();
    const float timeLR = IRPCDigiTime(*digi).timeLR();

    auto& pairCont = signalsByBx[bunchX];
    pairCont.first.push_back(IRPCSignal{strip, timeHR, bunchX, true, false});
    pairCont.second.push_back(IRPCSignal{strip, timeLR, bunchX, false, true});
  }

  for (auto& entry : signalsByBx) {
    auto& signalContHR = entry.second.first;
    auto& signalContLR = entry.second.second;

    std::vector<IRPCOneSideCluster> clustersHR;
    std::vector<IRPCOneSideCluster> clustersLR;
    (void)IRPCOneSideClusterizer(thrTime, signalContHR, clustersHR);
    (void)IRPCOneSideClusterizer(thrTime, signalContLR, clustersLR);

    std::vector<IRPCOneSideCluster> finalClusters;
    if (!clustersHR.empty() && !clustersLR.empty()) {
      finalClusters = IRPCFinalClusterizer(clustersHR, clustersLR, thrStripNum);
    }

    for (const auto& finalCluster : finalClusters) {
      int fstrip = std::numeric_limits<int>::max();
      int lstrip = std::numeric_limits<int>::min();
      int bx = 0;

      uint16_t nHigh = 0;
      float sumHigh = 0.f;
      float sumHigh2 = 0.f;
      uint16_t nLow = 0;
      float sumLow = 0.f;
      float sumLow2 = 0.f;

      for (std::size_t i = 0; i < finalCluster.signals.size(); ++i) {
        const IRPCSignal& s = finalCluster.signals[i];
        fstrip = std::min(fstrip, s.strip);
        lstrip = std::max(lstrip, s.strip);
        if (i == 0) {
          bx = s.bx;
        }
        if (s.hr) {
          ++nHigh;
          sumHigh += s.time;
          sumHigh2 += s.time * s.time;
        }
        if (s.lr) {
          ++nLow;
          sumLow += s.time;
          sumLow2 += s.time * s.time;
        }
      }

      // Local y: 0.5 * (mean LR - mean HR) * speed — same convention as IRPCDigiTime::coordinateY ( (LR-HR)/2 ).
      uint16_t nY = 0;
      float sumY = 0.f;
      float sumY2 = 0.f;
      if (nHigh > 0 && nLow > 0) {
        const float meanHR = sumHigh / static_cast<float>(nHigh);
        const float meanLR = sumLow / static_cast<float>(nLow);
        const float y = 0.5f * (meanLR - meanHR) * speed;
        nY = 1;
        sumY = y;
        sumY2 = y * y;
      }

      IRPCCluster cl;
      cl.setClusterSummary(static_cast<uint16_t>(fstrip),
                           static_cast<uint16_t>(lstrip),
                           static_cast<int16_t>(bx),
                           nHigh,
                           sumHigh,
                           sumHigh2,
                           nLow,
                           sumLow,
                           sumLow2,
                           nY,
                           sumY,
                           sumY2);
      out.insert(cl);
    }
  }

  return out;
}

}  // namespace

IRPCClusterContainer IRPCClusterizer::doAction(const IRPCDigiCollection::Range& digiRange,
                                               float thrTime,
                                               float thrStripNum,
                                               float speed) const {
  return IRPCClusteringDoAction(digiRange, thrTime, thrStripNum, speed);
}
