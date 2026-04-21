/*
 * See header file for a description of this class.
 *
 * \author J. Shin -- Kyung Hee University
 */

#include "DataFormats/RPCDigi/interface/IRPCDigiTime.h"

#include "IRPCCluster.h"
#include "IRPCClusterizer.h"
#include "IRPCSignal.h"

#include <algorithm>
#include <cmath>
#include <iterator>
#include <limits>
#include <map>
#include <vector>

namespace {

bool oneSideClusterizer(float thrTime,
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
    const float seedTime = signalCont[signalFlags[minTimeIdx].first].time;
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
      if (isAdjacentStrip(leftSig.strip, refSig.strip, maxStripJump) &&
          isAdjacentTime(leftSig.time, seedTime, thrTime)) {
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
      if (isAdjacentStrip(rightSig.strip, refSig.strip, maxStripJump) &&
          isAdjacentTime(rightSig.time, seedTime, thrTime)) {
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
    const float stripHR = stripNumAvg(HR[iHR].signals);
    float minDeltaStr = std::numeric_limits<float>::infinity();
    std::size_t minIdxLr = LR.size();

    for (std::size_t iLR = 0; iLR < LR.size(); ++iLR) {
      if (usedLR[iLR]) {
        continue;
      }
      const float stripLR = stripNumAvg(LR[iLR].signals);
      const float deltaStrip = std::abs(stripHR - stripLR);
      if (deltaStrip < minDeltaStr) {
        minDeltaStr = deltaStrip;
        minIdxLr = iLR;
      }
    }

    // Strictly below threshold; each LR cluster used at most once (usedLR).
    if (minIdxLr < LR.size() && minDeltaStr < thrStripNum) {
      IRPCOneSideCluster merged;
      merged.signals = HR[iHR].signals;
      merged.signals.insert(merged.signals.end(), LR[minIdxLr].signals.begin(), LR[minIdxLr].signals.end());
      out.push_back(std::move(merged));
      usedLR[minIdxLr] = 1;
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
    (void)oneSideClusterizer(thrTime, signalContHR, clustersHR);
    (void)oneSideClusterizer(thrTime, signalContLR, clustersLR);

    std::vector<IRPCOneSideCluster> finalClusters;
    if (!clustersHR.empty() && !clustersLR.empty()) {
      finalClusters = IRPCFinalClusterizer(clustersHR, clustersLR, thrStripNum);
    }

    for (const auto& finalCluster : finalClusters) {
      out.insert(IRPCCluster::compute(finalCluster.signals, speed));
    }
  }

  return out;
}

}  // namespace

IRPCClusterizer::IRPCClusterizer(float thrTime, float thrStripNum, float speed)
    : thrTime_(thrTime), thrStripNum_(thrStripNum), speed_(speed) {}

IRPCClusterContainer IRPCClusterizer::doAction(const IRPCDigiCollection::Range& digiRange) const {
  return IRPCClusteringDoAction(digiRange, thrTime_, thrStripNum_, speed_);
}
