#ifndef RecoLocalMuon_RPCRecHit_IRPCCluster_h
#define RecoLocalMuon_RPCRecHit_IRPCCluster_h

/*
 * IRPCCluster — one local hit candidate after HR/LR clustering and matching.
 *
 * Built by IRPCCluster::compute() from a list of matched IRPCSignal entries.
 * Stores strip span, BX, HR/LR time sums, and optional local y.
 *
 * time()    : mean( 0.5*(t_HR_i + t_LR_i) ) over HR signals in the merged cluster
 * timeRMS() : std_dev of per-digi time — accounts for HR/LR correlation
 * y()       : mean( 0.5*(t_LR_i - t_HR_i)*speed ) over HR signals in the merged cluster
 * yRMS()    : std_dev of per-digi y — non-zero, physically meaningful timing spread
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
  static IRPCCluster compute(const std::vector<IRPCSignal>& signals);

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

  float sumTime_;
  float sumTime2_;
  uint16_t nTime_;

  float sumY_;
  float sumY2_;
  uint16_t nY_;
};

#endif
