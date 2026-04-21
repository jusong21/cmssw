#ifndef RecoLocalMuon_RPCRecHit_IRPCSignal_h
#define RecoLocalMuon_RPCRecHit_IRPCSignal_h

/*
 * One HR or LR endpoint measurement along a strip (not an EDM product).
 *
 * Built from IRPCDigi: strip, bx, time from IRPCDigiTime::timeHR()/timeLR(), and isHR/isLR
 * mark which endpoint the sample belongs to. A merged HR+LR hit candidate holds both kinds
 * of entries in one vector (see IRPCCluster.h: IRPCFinalCluster).
 *
 * \author J. Song -- Hanyang University, Vrije Universiteit Brussel
 */

#include <vector>

struct IRPCSignal {
  int strip = 0;
  float time = 0.f;
  int bx = 0;
  bool isHR = false;
  bool isLR = false;
};

bool isAdjacentStrip(int stripA, int stripB, int maxStripJump);
bool isAdjacentTime(float timeA, float timeB, float thrTime);

float stripNumAvg(const std::vector<IRPCSignal>& signals);

#endif
