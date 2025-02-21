#ifndef DataFormats_IRPCRecHit_H
#define DataFormats_IRPCRecHit_H

/** \class RPCRecHit
 *
 *  RecHit for RPC 
 *
 *  \author M. Maggi -- INFN Bari 
 */

#include "DataFormats/TrackingRecHit/interface/RecHit2DLocalPos.h"
#include "DataFormats/MuonDetId/interface/RPCDetId.h"

class IRPCRecHit : public RecHit2DLocalPos {
public:
  IRPCRecHit(const RPCDetId& rpcId, int bx);

  /// Default constructor
  IRPCRecHit();

  /// Constructor from a local position, rpcId and digi time.
  /// The 3-dimensional local error is defined as
  /// resolution (the cell resolution) for the coordinate being measured
  /// and 0 for the two other coordinates
  IRPCRecHit(const RPCDetId& rpcId, int bx, const LocalPoint& pos);

  /// Constructor from a local position and error, rpcId and bx.
  IRPCRecHit(const RPCDetId& rpcId, int bx, const LocalPoint& pos, const LocalError& err);

  /// Constructor from a local position and error, rpcId, bx, frist strip of cluster and cluster size.
  IRPCRecHit(const RPCDetId& rpcId, int bx, int firstStrip, int clustSize, const LocalPoint& pos, const LocalError& err);

  /// Destructor
  ~IRPCRecHit() override;

  /// Return the 3-dimensional local position
  LocalPoint localPosition() const override { return theLocalPosition; }

  /// Return the 3-dimensional error on the local position
  LocalError localPositionError() const override { return theLocalError; }

  IRPCRecHit* clone() const override;

  /// Access to component RecHits.
  /// No components rechits: it returns a null vector
  std::vector<const TrackingRecHit*> recHits() const override;

  /// Non-const access to component RecHits.
  /// No components rechits: it returns a null vector
  std::vector<TrackingRecHit*> recHits() override;

  /// Set local position
  void setPosition(LocalPoint pos) { theLocalPosition = pos; }

  /// Set local position error
  void setError(LocalError err) { theLocalError = err; }

  /// Set the local position and its error
  void setPositionAndError(LocalPoint pos, LocalError err) {
    theLocalPosition = pos;
    theLocalError = err;
  }

  /// Set the time and its error
  void setHRTimeAndError(float time, float err) {
    theTimeHR = time;
    theTimeHRError = err;
  }
  void setLRTimeAndError(float time, float err) {
    theTimeLR = time;
    theTimeLRError = err;
  }


  /// Return the rpcId
  RPCDetId rpcId() const { return theRPCId; }

  int BunchX() const { return theBx; }

  int firstClusterStrip() const { return theFirstStrip; }

  int clusterSize() const { return theClusterSize; }

  float timeHR() const { return theTimeHR; }

  float timeHRError() const { return theTimeHRError; }

  float timeLR() const { return theTimeLR; }

  float timeLRError() const { return theTimeLRError; }

  /// Comparison operator, based on the rpcId and the digi time
  bool operator==(const IRPCRecHit& hit) const;

private:
  RPCDetId theRPCId;
  int theBx;
  int theFirstStrip;
  int theClusterSize;
  // Position and error in the Local Ref. Frame of the RPCLayer
  LocalPoint theLocalPosition;
  LocalError theLocalError;
  float theTimeHR, theTimeHRError;
  float theTimeLR, theTimeLRError;
};
#endif

/// The ostream operator
std::ostream& operator<<(std::ostream& os, const IRPCRecHit& hit);
