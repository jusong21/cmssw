#ifndef RecoLocalMuon_RPCRecHit_IRPCSignal_h
#define RecoLocalMuon_RPCRecHit_IRPCSignal_h

/*
 * Intermediate representation for IRPC clustering (HR/LR digi signals).
 * Used only inside RPCRecHit plugins, not persisted as an EDM product.
 *
 * \author J. Song -- Hanyang University, Vrije Universiteit Brussel
 */

#include <vector>

struct IRPCSignal {
  int strip = 0;
  float time = 0.f;
  int bx = 0;
  bool hr = false;
  bool lr = false;
};

/// One-side (HR-only or LR-only) cluster built from ordered IRPCSignal entries.
struct IRPCOneSideCluster {
  std::vector<IRPCSignal> signals;
};

bool isAdjacentStrip(int stripA, int stripB, int maxStripJump);
bool isAdjacentTime(float timeA, float timeB, float thrTime);

float stripNumAvg(const std::vector<IRPCSignal>& signals);

#endif
