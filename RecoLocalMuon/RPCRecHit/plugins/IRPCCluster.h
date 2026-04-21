#ifndef RecoLocalMuon_RPCRecHit_IRPCCluster_h
#define RecoLocalMuon_RPCRecHit_IRPCCluster_h

/*
 * IRPCCluster — one local hit candidate after HR/LR clustering and matching.
 *
 * Built by IRPCCluster::compute() from a list of matched IRPCSignal entries.
 * Stores strip span, BX, HR/LR time sums, and optional local y.
 *
 * time()    : 0.5*(meanHR + meanLR)
 * timeRMS() : 0.5*sqrt(rmsHR^2 + rmsLR^2)
 * y()       : 0.5*(meanLR - meanHR)*speed  [set only when both HR and LR samples exist]
 *
 * operator< and operator== are required for std::set<IRPCCluster> (IRPCClusterContainer).
 *
 * \author J. Shin -- Kyung Hee University
 */

#include <cstdint>
#include <vector>

#include "IRPCSignal.h"

class IRPCCluster {
public:
  IRPCCluster();
  ~IRPCCluster();

  int firstStrip() const;
  int lastStrip() const;
  int clusterSize() const;
  int bx() const;

  /// Builds cluster from matched HR+LR signals (output of IRPCFinalClusterizer).
  static IRPCCluster compute(const std::vector<IRPCSignal>& signals, float speed);

  bool hasTime() const;
  float time() const;
  float timeRMS() const;

  bool hasY() const;
  float y() const;
  float yRMS() const;

  bool operator<(const IRPCCluster& other) const;
  bool operator==(const IRPCCluster& other) const;

private:
  uint16_t fstrip_;
  uint16_t lstrip_;
  int16_t bx_;

  uint16_t nHR_;
  float sumHR_;
  float sumHR2_;
  uint16_t nLR_;
  float sumLR_;
  float sumLR2_;

  float sumY_;
  float sumY2_;
  uint16_t nY_;
};

#endif
