/** \file
 *
 *  \author M. Maggi -- INFN Bari
*/

#include "RecoLocalMuon/IRPCRecHit/plugins/IRPCRecHitProducer.h"
#include "RecoLocalMuon/IRPCRecHit/interface/IRPCInfo.h"

#include "Geometry/RPCGeometry/interface/RPCRoll.h"
#include "DataFormats/MuonDetId/interface/RPCDetId.h"
#include "DataFormats/IRPCRecHit/interface/IRPCRecHit.h"

#include "RecoLocalMuon/IRPCRecHit/interface/IRPCRecHitAlgoFactory.h"
#include "DataFormats/IRPCRecHit/interface/IRPCRecHitCollection.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include <string>
#include <fstream>

using namespace edm;
using namespace std;

IRPCRecHitProducer::IRPCRecHitProducer(const ParameterSet& config)
    : theIRPCDigiLabel(consumes<IRPCDigiCollection>(config.getParameter<InputTag>("irpcDigiLabel"))),
      theRPCGeomToken(esConsumes()),
      // Get the concrete reconstruction algo from the factory
      theAlgo{IRPCRecHitAlgoFactory::get()->create(config.getParameter<string>("recAlgo"),
                                                   config.getParameter<ParameterSet>("recAlgoConfig"))} {
//      maskSource_(MaskSource::EventSetup),
//      deadSource_(MaskSource::EventSetup) {
  // Set verbose output
  produces<IRPCRecHitCollection>();

  // Get threshold time and strip number
  thrTime = config.getParameter<double>("thrTime");
  thrStripNum = config.getParameter<double>("thrStripNum");

//  // Get masked- and dead-strip information
//  theIRPCMaskedStripsObj = std::make_unique<IRPCMaskedStrips>();
//  theIRPCDeadStripsObj = std::make_unique<IRPCDeadStrips>();
//
//  const string maskSource = config.getParameter<std::string>("maskSource");
//  if (maskSource == "File") {
//    maskSource_ = MaskSource::File;
//    edm::FileInPath fp = config.getParameter<edm::FileInPath>("maskvecfile");
//    std::ifstream inputFile(fp.fullPath().c_str(), std::ios::in);
//    if (!inputFile) {
//      std::cerr << "Masked Strips File cannot not be opened" << std::endl;
//      exit(1);
//    }
//    while (inputFile.good()) {
//      IRPCMaskedStrips::MaskItem Item;
//      inputFile >> Item.rawId >> Item.strip;
//      if (inputFile.good())
//        MaskVec.push_back(Item);
//    }
//    inputFile.close();
//  } else {
//    theReadoutMaskedStripsToken = esConsumes();
//  }
//
//  const string deadSource = config.getParameter<std::string>("deadSource");
//  if (deadSource == "File") {
//    deadSource_ = MaskSource::File;
//    edm::FileInPath fp = config.getParameter<edm::FileInPath>("deadvecfile");
//    std::ifstream inputFile(fp.fullPath().c_str(), std::ios::in);
//    if (!inputFile) {
//      std::cerr << "Dead Strips File cannot not be opened" << std::endl;
//      exit(1);
//    }
//    while (inputFile.good()) {
//      IRPCDeadStrips::DeadItem Item;
//      inputFile >> Item.rawId >> Item.strip;
//      if (inputFile.good())
//        DeadVec.push_back(Item);
//    }
//    inputFile.close();
//  } else {
//    theReadoutDeadStripsToken = esConsumes();
//  }
}

IRPCRecHitProducer::~IRPCRecHitProducer() = default;

//void IRPCRecHitProducer::beginRun(const edm::Run& r, const edm::EventSetup& setup) {
//  // Getting the masked-strip information
//  if (maskSource_ == MaskSource::EventSetup) {
//    theIRPCMaskedStripsObj->MaskVec = setup.getData(theReadoutMaskedStripsToken).MaskVec;
//  } else if (maskSource_ == MaskSource::File) {
//    std::vector<IRPCMaskedStrips::MaskItem>::iterator posVec;
//    for (posVec = MaskVec.begin(); posVec != MaskVec.end(); ++posVec) {
//      IRPCMaskedStrips::MaskItem Item;
//      Item.rawId = (*posVec).rawId;
//      Item.strip = (*posVec).strip;
//      theIRPCMaskedStripsObj->MaskVec.push_back(Item);
//    }
//  }
//
//  // Getting the dead-strip information
//  if (deadSource_ == MaskSource::EventSetup) {
//    theIRPCDeadStripsObj->DeadVec = setup.getData(theReadoutDeadStripsToken).DeadVec;
//  } else if (deadSource_ == MaskSource::File) {
//    std::vector<IRPCDeadStrips::DeadItem>::iterator posVec;
//    for (posVec = DeadVec.begin(); posVec != DeadVec.end(); ++posVec) {
//      IRPCDeadStrips::DeadItem Item;
//      Item.rawId = (*posVec).rawId;
//      Item.strip = (*posVec).strip;
//      theIRPCDeadStripsObj->DeadVec.push_back(Item);
//    }
//  }
//}

void IRPCRecHitProducer::produce(Event& event, const EventSetup& setup) {
  // Get the RPC Geometry
  auto const& rpcGeom = setup.getData(theRPCGeomToken);

  // Get the digis from the event
  Handle<IRPCDigiCollection> digis;
  event.getByToken(theIRPCDigiLabel, digis);

  // Pass the EventSetup to the algo
  theAlgo->setES(setup);

  // Create the pointer to the collection which will store the rechits
  auto recHitCollection = std::make_unique<IRPCRecHitCollection>();

  // Iterate through all digi collections ordered by LayerId

  for (auto rpcdgIt = digis->begin(); rpcdgIt != digis->end(); ++rpcdgIt) {
    // The layerId
    const RPCDetId& rpcId = (*rpcdgIt).first;

    // Get the GeomDet from the setup
    const RPCRoll* roll = rpcGeom.roll(rpcId);
    if (roll == nullptr) {
      edm::LogError("BadDigiInput") << "Failed to find RPCRoll for ID " << rpcId;
      continue;
    }

    // Get the iterators over the digis associated with this LayerId
    const IRPCDigiCollection::Range& range = (*rpcdgIt).second;

    // Getting the roll mask, that includes dead strips, for the given IRPCDet
//    RollMask mask;
//    const int rawId = rpcId.rawId();
//    for (const auto& tomask : theIRPCMaskedStripsObj->MaskVec) {
//      if (tomask.rawId == rawId) {
//        const int bit = tomask.strip;
//        mask.set(bit - 1);
//      }
//    }
//
//    for (const auto& tomask : theIRPCDeadStripsObj->DeadVec) {
//      if (tomask.rawId == rawId) {
//        const int bit = tomask.strip;
//        mask.set(bit - 1);
//      }
//    }

	// Get the IRPC Info
	IRPCInfo info;
	info.setThrTimeHR( thrTime );
	info.setThrTimeLR( thrTime );
	info.setThrStripNum( thrStripNum );

    // Call the reconstruction algorithm
//    OwnVector<IRPCRecHit> recHits = theAlgo->reconstruct(*roll, rpcId, range, mask);
    OwnVector<IRPCRecHit> recHits = theAlgo->reconstruct(*roll, rpcId, range, info);

    if (!recHits.empty())  //FIXME: is it really needed?
      recHitCollection->put(rpcId, recHits.begin(), recHits.end());
  }

  event.put(std::move(recHitCollection));
}
