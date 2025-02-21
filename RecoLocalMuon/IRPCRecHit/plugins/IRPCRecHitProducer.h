#ifndef RecoLocalMuon_IRPCRecHitProducer_h
#define RecoLocalMuon_IRPCRecHitProducer_h

/** \class RPCRecHitProducer
 *  Module for RPCRecHit production. 
 *  
 *  \author M. Maggim -- INFN Bari
 */

#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/IRPCDigi/interface/IRPCDigiCollection.h"
#include "Geometry/RPCGeometry/interface/RPCGeometry.h"
#include "Geometry/Records/interface/MuonGeometryRecord.h"
#include "RecoLocalMuon/IRPCRecHit/interface/IRPCRecHitBaseAlgo.h"

class IRPCRecHitProducer : public edm::stream::EDProducer<> {
public:
  /// Constructor
  IRPCRecHitProducer(const edm::ParameterSet& config);

  /// Destructor
  ~IRPCRecHitProducer() override;

  // Method that access the EventSetup for each run
  //void beginRun(const edm::Run&, const edm::EventSetup&) override;

  /// The method which produces the rechits
  void produce(edm::Event& event, const edm::EventSetup& setup) override;

private:
  // The label to be used to retrieve IRPC digis from the event
  const edm::EDGetTokenT<IRPCDigiCollection> theIRPCDigiLabel;
  //  edm::InputTag theIRPCDigiLabel;
  //edm::ESGetToken<RPCMaskedStrips, RPCMaskedStripsRcd> theReadoutMaskedStripsToken;
  //edm::ESGetToken<RPCDeadStrips, RPCDeadStripsRcd> theReadoutDeadStripsToken;
  const edm::ESGetToken<RPCGeometry, MuonGeometryRecord> theRPCGeomToken;

  // The reconstruction algorithm
  std::unique_ptr<IRPCRecHitBaseAlgo> theAlgo;

  double thrTime;
  double thrStripNum;

//  std::unique_ptr<RPCMaskedStrips> theRPCMaskedStripsObj;
//  // Object with mask-strips-vector for all the RPC Detectors
//
//  std::unique_ptr<RPCDeadStrips> theRPCDeadStripsObj;
//  // Object with dead-strips-vector for all the RPC Detectors
//
//  enum class MaskSource { File, EventSetup } maskSource_, deadSource_;
//
//  std::vector<RPCMaskedStrips::MaskItem> MaskVec;
//  std::vector<RPCDeadStrips::DeadItem> DeadVec;
};

#endif
