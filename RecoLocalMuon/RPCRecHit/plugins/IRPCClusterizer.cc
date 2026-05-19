/*
 * IRPCClusterizer: digis -> IRPCCluster set.
 *
 * Per roll range: group IRPCSignal by BX (HR list, LR list from IRPCDigiTime).
 * oneSideClusterizer grows strip/time-adjacent groups on each list separately.
 * finalClusterizer pairs each HR-side cluster with the closest unused LR-side cluster
 * by strip position (threshold thrStripNum), producing a merged signal list.
 * IRPCCluster::compute folds each merged list into one IRPCCluster (strip span, sums, y).
 *
 * \author J. Shin -- Kyung Hee University
 */

#include "DataFormats/RPCDigi/interface/IRPCDigiTime.h"

#include "IRPCCluster.h"
#include "IRPCClusterizer.h"

#include <algorithm>
#include <cmath>
#include <iterator>
#include <limits>
#include <map>
#include <vector>

namespace {

// Groups strip/time-adjacent signals into clusters (one readout side at a time).
// Signals are consumed in order of increasing time; each signal is assigned to at most one cluster.
std::vector<std::vector<IRPCSignal>> oneSideClusterizer(float thrTime,
                                                         std::vector<IRPCSignal>& signalCont) {
  std::vector<std::vector<IRPCSignal>> clusters;
  if (signalCont.empty()) {
    return clusters;
  }

  std::vector<bool> used(signalCont.size(), false);

  while (true) {
    // Find the earliest unused signal as the seed.
    float minTime = std::numeric_limits<float>::infinity();
    std::size_t seedIdx = signalCont.size();
    for (std::size_t i = 0; i < signalCont.size(); ++i) {
      if (!used[i] && signalCont[i].oneSideTime < minTime) {
        minTime = signalCont[i].oneSideTime;
        seedIdx = i;
      }
    }
    if (seedIdx == signalCont.size()) {
      break;
    }

    std::vector<IRPCSignal> cluster;
    cluster.push_back(signalCont[seedIdx]);
    used[seedIdx] = true;

    const int maxStripJump = 1;
    const float seedTime = signalCont[seedIdx].oneSideTime;

    // Expand left.
    std::size_t cur = seedIdx;
    while (cur > 0) {
      const std::size_t left = cur - 1;
      if (isAdjacentStrip(signalCont[left].strip, signalCont[cur].strip, maxStripJump) &&
          isAdjacentTime(signalCont[left].oneSideTime, seedTime, thrTime)) {
        cluster.push_back(signalCont[left]);
        used[left] = true;
        cur = left;
      } else {
        break;
      }
    }

    // Expand right.
    cur = seedIdx;
    while (cur + 1 < signalCont.size()) {
      const std::size_t right = cur + 1;
      if (isAdjacentStrip(signalCont[right].strip, signalCont[cur].strip, maxStripJump) &&
          isAdjacentTime(signalCont[right].oneSideTime, seedTime, thrTime)) {
        cluster.push_back(signalCont[right]);
        used[right] = true;
        cur = right;
      } else {
        break;
      }
    }

    clusters.push_back(std::move(cluster));
  }
  return clusters;
}

// Matches each HR-side cluster to the nearest unused LR-side cluster by average strip position.
std::vector<std::vector<IRPCSignal>> finalClusterizer(
    const std::vector<std::vector<IRPCSignal>>& hrClusters,
    const std::vector<std::vector<IRPCSignal>>& lrClusters,
    float thrStripNum) {
  std::vector<std::vector<IRPCSignal>> out;
  std::vector<bool> usedLR(lrClusters.size(), false);

  for (std::size_t iHR = 0; iHR < hrClusters.size(); ++iHR) {
    const float stripHR = stripNumAvg(hrClusters[iHR]);
    float minDelta = std::numeric_limits<float>::infinity();
    std::size_t bestLR = lrClusters.size();

    for (std::size_t iLR = 0; iLR < lrClusters.size(); ++iLR) {
      if (usedLR[iLR]) {
        continue;
      }
      const float delta = std::abs(stripHR - stripNumAvg(lrClusters[iLR]));
      if (delta < minDelta) {
        minDelta = delta;
        bestLR = iLR;
      }
    }

    if (bestLR < lrClusters.size() && minDelta < thrStripNum) {
      std::vector<IRPCSignal> merged = hrClusters[iHR];
      merged.insert(merged.end(), lrClusters[bestLR].begin(), lrClusters[bestLR].end());
      out.push_back(std::move(merged));
      usedLR[bestLR] = true;
    }
  }

  return out;
}

IRPCClusterContainer clusteringDoAction(const IRPCDigiCollection::Range& digiRange,
                                            float thrTime,
                                            float thrStripNum) {
  IRPCClusterContainer out;

  if (std::distance(digiRange.first, digiRange.second) == 0) {
    return out;
  }

  std::map<int, std::pair<std::vector<IRPCSignal>, std::vector<IRPCSignal>>> signalsByBx;

  for (auto digi = digiRange.first; digi != digiRange.second; ++digi) {
    const int bunchX = digi->bx();
    const int strip = digi->strip();
    IRPCDigiTime dt(*digi);
    const float timeHR  = dt.timeHR();
    const float timeLR  = dt.timeLR();
    const float hitTime = dt.time();          // 0.5*(t_HR + t_LR)
    const float hitY    = dt.coordinateY();   // 0.5*(t_LR - t_HR)*speed

    auto& pair = signalsByBx[bunchX];
    pair.first.push_back(IRPCSignal{strip, timeHR, bunchX, true, false, hitTime, hitY});   // HR side
    pair.second.push_back(IRPCSignal{strip, timeLR, bunchX, false, true, hitTime, hitY});  // LR side
  }

  for (auto& entry : signalsByBx) {
    auto& signalsHR = entry.second.first;
    auto& signalsLR = entry.second.second;

    const auto clustersHR = oneSideClusterizer(thrTime, signalsHR);
    const auto clustersLR = oneSideClusterizer(thrTime, signalsLR);

    if (clustersHR.empty() || clustersLR.empty()) {
      continue;
    }

    const auto finalClusters = finalClusterizer(clustersHR, clustersLR, thrStripNum);
    for (const auto& finalCluster : finalClusters) {
      out.insert(IRPCCluster::compute(finalCluster));
    }
  }

  return out;
}

}  // namespace

IRPCClusterizer::IRPCClusterizer(float thrTime, float thrStripNum)
    : thrTime_(thrTime), thrStripNum_(thrStripNum) {}

IRPCClusterContainer IRPCClusterizer::doAction(const IRPCDigiCollection::Range& digiRange) const {
  return clusteringDoAction(digiRange, thrTime_, thrStripNum_);
}
