#ifndef RecoLocalMuon_RPCRecHit_IRPCClusterizer_h
#define RecoLocalMuon_RPCRecHit_IRPCClusterizer_h

/*
 * IRPCDigi -> IRPCClusterContainer. Pipeline: digis by BX -> one-side clusters (HR/LR) ->
 * HR–LR match -> IRPCCluster::compute (see IRPCCluster.h).
 *
 * \author J. Shin -- Kyung Hee University
 */

#include "DataFormats/RPCDigi/interface/IRPCDigiCollection.h"

#include "IRPCCluster.h"
#include "IRPCClusterContainer.h"

class IRPCClusterizer {
public:
  IRPCClusterizer(float thrTime, float thrStripNum);
  ~IRPCClusterizer() = default;

  IRPCClusterContainer doAction(const IRPCDigiCollection::Range& digiRange) const;

private:
  float thrTime_;
  float thrStripNum_;
};

#endif