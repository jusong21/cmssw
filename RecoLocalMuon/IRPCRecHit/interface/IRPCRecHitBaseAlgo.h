#ifndef RecoLocalMuon_IRPCRecHitBaseAlgo_H
#define RecoLocalMuon_IRPCRecHitBaseAlgo_H

/** \class IRPCRecHitBaseAlgo
 *  Abstract algorithmic class to compute Rec Hit
 *  form a IRPC digi
 *
 *  \author M. Maggi -- INFN Bari
 *  \adapted by Juhee Song (Hanyang Univ, VUB)
 */

#include "DataFormats/GeometryVector/interface/LocalPoint.h"
#include "DataFormats/GeometrySurface/interface/LocalError.h"
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"
#include "DataFormats/RPCDigi/interface/IRPCDigiCollection.h"
#include "DataFormats/IRPCRecHit/interface/IRPCRecHit.h"
#include "RecoLocalMuon/IRPCRecHit/interface/IRPCInfo.h"
#include "DataFormats/Common/interface/OwnVector.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

class IRPCCluster;
class RPCRoll;
class RPCDetId;

namespace edm {
  class EventSetup;
}

class IRPCRecHitBaseAlgo {
public:
  /// Constructor
  IRPCRecHitBaseAlgo(const edm::ParameterSet& config);

  /// Destructor
  virtual ~IRPCRecHitBaseAlgo(){};

  /// Pass the Event Setup to the algo at each event
  virtual void setES(const edm::EventSetup& setup) = 0;

  /// Build all hits in the range associated to the rpcId, at the 1st step.
  virtual edm::OwnVector<IRPCRecHit> reconstruct(const RPCRoll& roll,
                                                 const RPCDetId& rpcId,
                                                 const IRPCDigiCollection::Range& digiRange,
                                                 //const RollMask& mask);
                                                 IRPCInfo& info);

  /// standard local recHit computation
  virtual bool compute(const RPCRoll& roll,
                       const IRPCCluster& cl,
                       LocalPoint& Point,
                       LocalError& error,
                       float& timeHR,
                       float& timeHRErr,
                       float& timeLR,
                       float& timeLRErr) const = 0;

  /// local recHit computation accounting for track direction and
  /// absolute position
  virtual bool compute(const RPCRoll& roll,
                       const IRPCCluster& cl,
                       const float& angle,
                       const GlobalPoint& globPos,
                       LocalPoint& Point,
                       LocalError& error,
                       float& timeHR,
                       float& timeHRErr,
                       float& timeLR,
                       float& timeLRErr) const = 0;
};
#endif
