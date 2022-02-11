/*
 *  See header file for a description of this class.
 *
 *  \author M. Maggi -- INFN Bari, Shchablo -- IPNL Lyon
 */

#include "RPCRecHitBaseAlgo.h"
#include "RPCClusterContainer.h"
#include "RPCCluster.h"
#include "RPCClusterizer.h"
#include "RPCMaskReClusterizer.h"
#include "Geometry/RPCGeometry/interface/RPCRoll.h"

/* iRPC */
#include "iRPCClusterContainer.h"		//moved to plugin directory
#include "iRPCCluster.h"
#include "iRPCClusterizer.h"


RPCRecHitBaseAlgo::RPCRecHitBaseAlgo(const edm::ParameterSet& config) {
  //  theSync = RPCTTrigSyncFactory::get()->create(config.getParameter<string>("tTrigMode"),
  //config.getParameter<ParameterSet>("tTrigModeConfig"));

   /* iRPC Info */
   edm::ParameterSet iRPCParams = config.getParameter<edm::ParameterSet>("iRPCConfig");
   // ---
   iRPCConfig.isUseIRPCAlgorithm(iRPCParams.getParameter<bool>("useAlgorithm"));
   iRPCConfig.isReturnOnlyAND(iRPCParams.getParameter<bool>("returnOnlyAND"));
   iRPCConfig.isOnlyHR(iRPCParams.getParameter<bool>("returnOnlyHR"));
   iRPCConfig.isOnlyLR(iRPCParams.getParameter<bool>("returnOnlyLR"));
   iRPCConfig.setSpeed(iRPCParams.getParameter<double>("signalSpeed")); //  conversion to float
   iRPCConfig.setThrTimeHR(iRPCParams.getParameter<double>("thrTimeHR")); // conversion to float
   iRPCConfig.setThrTimeLR(iRPCParams.getParameter<double>("thrTimeLR")); //  conversion to float
   iRPCConfig.setThrDeltaTimeMin(iRPCParams.getParameter<double>("thrDeltaTimeMin")); // conversion to float
   iRPCConfig.setThrDeltaTimeMax(iRPCParams.getParameter<double>("thrDeltaTimeMax")); //  conversion to float
   iRPCConfig.setThrDeltaTimeY(iRPCParams.getParameter<double>("thrDeltaTimeY")); //  conversion to float
   // test iRPC params
   //std::cout << "use: " << iRPCConfig.isUseIRPCAlgorithm() << " and: " << iRPCConfig.isReturnOnlyAND() << " speed: " << iRPCConfig.speed() <<
   //" thrHR: " << iRPCConfig.thrTimeHR() << " thrLR: " << iRPCConfig.thrTimeLR() << 
   //" thrMin: " << iRPCConfig.thrDeltaTimeMin() <<  " thrMax: " <<  iRPCConfig.thrDeltaTimeMax() << std::endl;
   // ---
}

// Build all hits in the range associated to the layerId, at the 1st step.
edm::OwnVector<RPCRecHit> RPCRecHitBaseAlgo::reconstruct(const RPCRoll& roll,
                                                         const RPCDetId& rpcId,
                                                         const RPCDigiCollection::Range& digiRange,
                                                         const RollMask& mask) {
  edm::OwnVector<RPCRecHit> result;
 
  if(iRPCConfig.isUseIRPCAlgorithm() && roll.isIRPC()) {
    /* iRPC Clustering */
    iRPCClusterizer clizer;
    iRPCClusterContainer cls = clizer.doAction(roll, digiRange, iRPCConfig);
    
    for ( auto cl : cls ) {
      LocalError tmpErr;
      LocalPoint point;
      float time = 0, timeErr = -1;
      
      // Call the compute method
      const bool OK = this->compute(roll, iRPCConfig, cl, point, tmpErr, time, timeErr);
      if (!OK) continue;

      // Build a new pair of 1D rechit
      const int firstClustStrip = cl.firstStrip();
      const int clusterSize = cl.clusterSize();
      RPCRecHit* recHit = new RPCRecHit(rpcId, cl.bx(), firstClustStrip, clusterSize, point, tmpErr);
      recHit->setTimeAndError(time, timeErr);

      result.push_back(recHit);
    }
  }
  else {
    
    /* RPC Clustering */
    RPCClusterizer clizer;
    RPCClusterContainer tcls = clizer.doAction(digiRange);
    RPCMaskReClusterizer mrclizer;
    RPCClusterContainer cls = mrclizer.doAction(rpcId,tcls,mask);

    for ( auto cl : cls ) {
      LocalError tmpErr;
      LocalPoint point;
      float time = 0, timeErr = -1;

      // Call the compute method
      const bool OK = this->compute(roll, cl, point, tmpErr, time, timeErr);
      if (!OK) continue;

      // Build a new pair of 1D rechit
      const int firstClustStrip = cl.firstStrip();
      const int clusterSize = cl.clusterSize();
      RPCRecHit* recHit = new RPCRecHit(rpcId,cl.bx(), firstClustStrip, clusterSize, point, tmpErr);
      recHit->setTimeAndError(time, timeErr);

      result.push_back(recHit);
    }
  }

/*
  RPCClusterizer clizer;
  RPCClusterContainer tcls = clizer.doAction(digiRange);
  RPCMaskReClusterizer mrclizer;
  RPCClusterContainer cls = mrclizer.doAction(rpcId, tcls, mask);

  for (const auto& cl : cls) {
    LocalError tmpErr;
    LocalPoint point;
    float time = 0, timeErr = -1;

    // Call the compute method
    const bool OK = this->compute(roll, cl, point, tmpErr, time, timeErr);
    if (!OK)
      continue;

    // Build a new pair of 1D rechit
    const int firstClustStrip = cl.firstStrip();
    const int clusterSize = cl.clusterSize();
    RPCRecHit* recHit = new RPCRecHit(rpcId, cl.bx(), firstClustStrip, clusterSize, point, tmpErr);
    recHit->setTimeAndError(time, timeErr);

    result.push_back(recHit);
  }
*/
  return result;
}
