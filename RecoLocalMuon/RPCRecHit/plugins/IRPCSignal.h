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
  float oneSideTime = 0.f;  // endpoint time for clustering: t_HR if isHR, t_LR if isLR
  int bx = 0;
  bool isHR = false;
  bool isLR = false;
  float time = 0.f;  // 0.5*(t_HR + t_LR) — hit time from this digi
  float y = 0.f;     // 0.5*(t_LR - t_HR)*speed — hit y from this digi
};

bool isAdjacentStrip(int stripA, int stripB, int maxStripJump);
bool isAdjacentTime(float timeA, float timeB, float thrTime);

float stripNumAvg(const std::vector<IRPCSignal>& signals);

#endif
