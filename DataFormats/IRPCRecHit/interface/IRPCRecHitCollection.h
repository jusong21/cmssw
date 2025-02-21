#ifndef DataFormats_IRPCRecHitCollection_H
#define DataFormats_IRPCRecHitCollection_H

/** \class IRPCRecHitCollection
 *  Collection of IRPCRecHit for storage in the event
 *
 *  \author M. Maggi - INFN Bari
 */

#include "DataFormats/MuonDetId/interface/RPCDetId.h"
#include "DataFormats/IRPCRecHit/interface/IRPCRecHit.h"
#include "DataFormats/Common/interface/RangeMap.h"
#include "DataFormats/Common/interface/ClonePolicy.h"
#include "DataFormats/Common/interface/OwnVector.h"
#include <functional>

typedef edm::RangeMap<RPCDetId, edm::OwnVector<IRPCRecHit, edm::ClonePolicy<IRPCRecHit> >, edm::ClonePolicy<IRPCRecHit> >
    IRPCRecHitCollection;

#endif
