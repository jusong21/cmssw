#ifndef RecoLocalMuon_IRPCRecHitStandardAlgo_H
#define RecoLocalMuon_IRPCRecHitStandardAlgo_H

/** \class RPCRecHitStandardAlgo
 *  Concrete implementation of RPCRecHitBaseAlgo.
 *
 *  \author M. Maggi -- INFN Bari
 */

#include "RecoLocalMuon/IRPCRecHit/interface/IRPCRecHitBaseAlgo.h"

class IRPCRecHitStandardAlgo : public IRPCRecHitBaseAlgo {
public:
  /// Constructor
  IRPCRecHitStandardAlgo(const edm::ParameterSet& config) : IRPCRecHitBaseAlgo(config){};

  /// Destructor
  ~IRPCRecHitStandardAlgo() override{};

  /// Pass the Event Setup to the algo at each event
  void setES(const edm::EventSetup& setup) override{};

  bool compute(const RPCRoll& roll,
               const IRPCCluster& cluster,
               LocalPoint& point,
               LocalError& error,
               float& timeHR,
               float& timeHRErr,
               float& timeLR,
               float& timeLRErr) const override;

  bool compute(const RPCRoll& roll,
               const IRPCCluster& cluster,
               const float& angle,
               const GlobalPoint& globPos,
               LocalPoint& point,
               LocalError& error,
               float& timeHR,
               float& timeHRErr,
               float& timeLR,
               float& timeLRErr) const override;
};
#endif
