/*
 *  See header file for a description of this class.
 *
 *  \author original version:  M. Maggi -- INFN Bari
 *  \adated by Juhee Song (Hanyang Univ, Vrije Universiteit Brussel)
 */

#include "RecoLocalMuon/RPCRecHit/interface/IRPCRecHitBaseAlgo.h"
#include "RecoLocalMuon/RPCRecHit/interface/IRPCClusterContainer.h"
#include "RecoLocalMuon/RPCRecHit/interface/IRPCCluster.h"
#include "RecoLocalMuon/RPCRecHit/interface/IRPCClusterizer.h"
//#include "RecoLocalMuon/RPCRecHit/interface/IRPCMaskReClusterizer.h"

IRPCRecHitBaseAlgo::IRPCRecHitBaseAlgo(const edm::ParameterSet& config) {
  //  theSync = IRPCTTrigSyncFactory::get()->create(config.getParameter<string>("tTrigMode"),
  //config.getParameter<ParameterSet>("tTrigModeConfig"));
}

// Build all hits in the range associated to the layerId, at the 1st step.
edm::OwnVector<IRPCRecHit> IRPCRecHitBaseAlgo::reconstruct(const RPCRoll& roll,
                                                           const RPCDetId& rpcId,
                                                           const IRPCDigiCollection::Range& digiRange,
														   IRPCInfo& info) {
                                                         //const RollMask& mask) {
  edm::OwnVector<IRPCRecHit> result;

  IRPCClusterizer clizer;
//  IRPCClusterContainer tcls = clizer.doAction(digiRange);
  IRPCClusterContainer cls = clizer.doAction(digiRange, info);

//  IRPCMaskReClusterizer mrclizer;
 // IRPCClusterContainer cls = mrclizer.doAction(rpcId, tcls, mask);

  for (const auto& cl : cls) {
    LocalError tmpErr;
    LocalPoint point;
    float timeHR = 0, timeHRErr = -1;
    float timeLR = 0, timeLRErr = -1;

    // Call the compute method
    const bool OK = this->compute(roll, cl, point, tmpErr, timeHR, timeHRErr, timeLR, timeLRErr);
    if (!OK)
      continue;

    // Build a new pair of 1D rechit
    const int firstClustStrip = cl.firstStrip();
    const int clusterSize = cl.clusterSize();
    IRPCRecHit* recHit = new IRPCRecHit(rpcId, cl.bx(), firstClustStrip, clusterSize, point, tmpErr);
    recHit->setHRTimeAndError(timeHR, timeHRErr);
    recHit->setLRTimeAndError(timeLR, timeLRErr);

    result.push_back(recHit);
  }

  return result;
}
