#ifndef RecoLocalMuon_RPCRecHit_IRPCClusterizer_h
#define RecoLocalMuon_RPCRecHit_IRPCClusterizer_h

/*
 * IRPCDigi: HR/LR one-side clustering, association -> IRPCCluster (see IRPCCluster::compute).
 *
 * \author J. Shin -- Kyung Hee University
 */

#include "DataFormats/RPCDigi/interface/IRPCDigiCollection.h"

#include "IRPCCluster.h"
#include "IRPCClusterContainer.h"

class IRPCClusterizer {
public:
  IRPCClusterizer() = default;
  ~IRPCClusterizer() = default;

  IRPCClusterContainer doAction(const IRPCDigiCollection::Range& digiRange,
                                  float thrTime,
                                  float thrStripNum,
                                  float speed) const;
};

#endif
