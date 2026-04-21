#ifndef RecoLocalMuon_RPCRecHit_IRPCCluster_h
#define RecoLocalMuon_RPCRecHit_IRPCCluster_h

/*
 * IRPC cluster: one merged HR+LR cluster from IRPCClusterizer (see compute).
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

  /// Strip range, bx, HR/LR time sums, local y from matched IRPCSignal list (after clusterizer).
  static IRPCCluster compute(const std::vector<IRPCSignal>& signals, float speed);

  bool hasHRTime() const;
  float hrTime() const;
  float hrTimeRMS() const;
  bool hasLRTime() const;
  float lrTime() const;
  float lrTimeRMS() const;

  /// RecHit time: 0.5*(meanHR + meanLR). RMS: error propagation from HR/LR RMS (see .cc).
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
