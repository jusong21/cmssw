/*
 *  See header file for a description of this class.
 *
 *  \author origianl version: M. Maggi -- INFN Bari
 *  \adated by Juhee Song (Hanyang Univ, Vrije Universiteit Brussel)
 */

#include "DataFormats/IRPCRecHit/interface/IRPCRecHit.h"

IRPCRecHit::IRPCRecHit(const RPCDetId& rpcId, int bx)
    : RecHit2DLocalPos(rpcId),
      theRPCId(rpcId),
      theBx(bx),
      theFirstStrip(99),
      theClusterSize(99),
      theLocalPosition(),
      theLocalError(),
      theTimeHR(0),
      theTimeHRError(-1),
      theTimeLR(0),
      theTimeLRError(-1) {}

IRPCRecHit::IRPCRecHit()
    : RecHit2DLocalPos(),
      theRPCId(),
      theBx(99),
      theFirstStrip(99),
      theClusterSize(99),
      theLocalPosition(),
      theLocalError(),
      theTimeHR(0),
      theTimeHRError(-1),
      theTimeLR(0),
      theTimeLRError(-1) {}

IRPCRecHit::IRPCRecHit(const RPCDetId& rpcId, int bx, const LocalPoint& pos)
    : RecHit2DLocalPos(rpcId),
      theRPCId(rpcId),
      theBx(bx),
      theFirstStrip(99),
      theClusterSize(99),
      theLocalPosition(pos),
      theTimeHR(0),
      theTimeHRError(-1),
      theTimeLR(0),
      theTimeLRError(-1) {
  float stripResolution = 3.0;  //cm  this sould be taken from trimmed cluster size times strip size
                                //    taken out from geometry service i.e. topology
  theLocalError = LocalError(stripResolution * stripResolution, 0., 0.);  //FIXME: is it really needed?
}

// Constructor from a local position and error, wireId and digi time.
IRPCRecHit::IRPCRecHit(const RPCDetId& rpcId, int bx, const LocalPoint& pos, const LocalError& err)
    : RecHit2DLocalPos(rpcId),
      theRPCId(rpcId),
      theBx(bx),
      theFirstStrip(99),
      theClusterSize(99),
      theLocalPosition(pos),
      theLocalError(err),
      theTimeHR(0),
      theTimeHRError(-1),
      theTimeLR(0),
      theTimeLRError(-1) {}

// Constructor from a local position and error, wireId, bx and cluster size.
IRPCRecHit::IRPCRecHit(
    const RPCDetId& rpcId, int bx, int firstStrip, int clustSize, const LocalPoint& pos, const LocalError& err)
    : RecHit2DLocalPos(rpcId),
      theRPCId(rpcId),
      theBx(bx),
      theFirstStrip(firstStrip),
      theClusterSize(clustSize),
      theLocalPosition(pos),
      theLocalError(err),
      theTimeHR(0),
      theTimeHRError(-1),
      theTimeLR(0),
      theTimeLRError(-1) {}

// Destructor
IRPCRecHit::~IRPCRecHit() {}

IRPCRecHit* IRPCRecHit::clone() const { return new IRPCRecHit(*this); }

// Access to component RecHits.
// No components rechits: it returns a null vector
std::vector<const TrackingRecHit*> IRPCRecHit::recHits() const {
  std::vector<const TrackingRecHit*> nullvector;
  return nullvector;
}

// Non-const access to component RecHits.
// No components rechits: it returns a null vector
std::vector<TrackingRecHit*> IRPCRecHit::recHits() {
  std::vector<TrackingRecHit*> nullvector;
  return nullvector;
}

// Comparison operator, based on the wireId and the digi time
bool IRPCRecHit::operator==(const IRPCRecHit& hit) const { return this->geographicalId() == hit.geographicalId(); }

// The ostream operator
std::ostream& operator<<(std::ostream& os, const IRPCRecHit& hit) {
  os << "pos: " << hit.localPosition().x();
  os << " +/- " << sqrt(hit.localPositionError().xx());
  return os;
}
