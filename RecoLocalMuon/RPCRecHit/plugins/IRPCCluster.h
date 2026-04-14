#ifndef RecoLocalMuon_RPCRecHit_IRPCCluster_h
#define RecoLocalMuon_RPCRecHit_IRPCCluster_h

/*
 * IRPC cluster after HR/LR association: HR and LR times (RMS) and y stored separately.
 *
 * \author J. Shin -- Kyung Hee University
 */

#include <cstdint>

class IRPCCluster {
public:
  IRPCCluster();
  ~IRPCCluster();

  int firstStrip() const;
  int lastStrip() const;
  int clusterSize() const;
  int bx() const;

  bool hasHighTime() const;
  float highTime() const;
  float highTimeRMS() const;

  bool hasLowTime() const;
  float lowTime() const;
  float lowTimeRMS() const;

  bool hasY() const;
  float y() const;
  float yRMS() const;

  void compute(uint16_t fstrip,
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
               float sumY2);

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

  uint16_t nY_;
  float sumY_;
  float sumY2_;
};

#endif
