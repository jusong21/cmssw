/** \file
 *
 *    \author original version: M. Maggi -- INFN Bari
 *    \adated by Juhee Song (Hanyang Univ, Vrije Universiteit Brussel)
 *
*/

#include "RecoLocalMuon/IRPCRecHit/plugins/IRPCRecHitProducer.h"
#include "RecoLocalMuon/IRPCRecHit/interface/IRPCInfo.h"

#include "Geometry/RPCGeometry/interface/RPCRoll.h"
#include "Geometry/RPCGeometry/interface/RPCGeometry.h"
#include "Geometry/Records/interface/MuonGeometryRecord.h"

#include "DataFormats/MuonDetId/interface/RPCDetId.h"
#include "DataFormats/IRPCRecHit/interface/IRPCRecHit.h"

#include "RecoLocalMuon/IRPCRecHit/interface/IRPCRecHitAlgoFactory.h"
#include "DataFormats/IRPCRecHit/interface/IRPCRecHitCollection.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include <string>
#include <fstream>

using namespace edm;
using namespace std;

IRPCRecHitProducer::IRPCRecHitProducer(const ParameterSet& iConfig)
    : rpcGeomToken(esConsumes<RPCGeometry, MuonGeometryRecord>()),
	  irpcDigiToken(consumes<IRPCDigiCollection>(iConfig.getParameter<InputTag>("irpcDigiTag"))),
      // Get the concrete reconstruction algo from the factory
      theAlgo{IRPCRecHitAlgoFactory::get()->create(iConfig.getParameter<string>("recAlgo"),
      iConfig.getParameter<ParameterSet>("recAlgoConfig"))} {
//            maskSource_(MaskSource::EventSetup),
//            deadSource_(MaskSource::EventSetup) {
      // Set verbose output
      produces<IRPCRecHitCollection>();

      // Get threshold time and strip number
      thrTime = iConfig.getParameter<double>("thrTime");
      thrStripNum = iConfig.getParameter<double>("thrStripNum");
	  //h1_NClusters_event = TH1D("h1_NClusters_event","N clusters per event",20,0,20);

}

IRPCRecHitProducer::~IRPCRecHitProducer() = default;

//void IRPCRecHitProducer::beginRun(const edm::Run& r, const edm::EventSetup& setup) {
//    // Getting the masked-strip information
//    if (maskSource_ == MaskSource::EventSetup) {
//        theIRPCMaskedStripsObj->MaskVec = setup.getData(theReadoutMaskedStripsToken).MaskVec;
//    } else if (maskSource_ == MaskSource::File) {
//        std::vector<IRPCMaskedStrips::MaskItem>::iterator posVec;
//        for (posVec = MaskVec.begin(); posVec != MaskVec.end(); ++posVec) {
//            IRPCMaskedStrips::MaskItem Item;
//            Item.rawId = (*posVec).rawId;
//            Item.strip = (*posVec).strip;
//            theIRPCMaskedStripsObj->MaskVec.push_back(Item);
//        }
//    }
//
//    // Getting the dead-strip information
//    if (deadSource_ == MaskSource::EventSetup) {
//        theIRPCDeadStripsObj->DeadVec = setup.getData(theReadoutDeadStripsToken).DeadVec;
//    } else if (deadSource_ == MaskSource::File) {
//        std::vector<IRPCDeadStrips::DeadItem>::iterator posVec;
//        for (posVec = DeadVec.begin(); posVec != DeadVec.end(); ++posVec) {
//            IRPCDeadStrips::DeadItem Item;
//            Item.rawId = (*posVec).rawId;
//            Item.strip = (*posVec).strip;
//            theIRPCDeadStripsObj->DeadVec.push_back(Item);
//        }
//    }
//}

void IRPCRecHitProducer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup) {


	std::cout << std::endl; 
	std::cout << "*************************" << std::endl;
    edm::LogVerbatim("RPCDump") << "**** new Run \n - Run: " << iEvent.id().run() << " \n - Event: " << iEvent.id().event();

    const auto& irpcDigis = iEvent.getHandle(irpcDigiToken);
    const auto& rpcGeom = iSetup.getHandle(rpcGeomToken);

    // Pass the EventSetup to the algo
    theAlgo->setES(iSetup);

    // Create the pointer to the collection which will store the rechits
    auto recHitCollection = std::make_unique<IRPCRecHitCollection>();

    // Iterate through all digi collections ordered by LayerId

	//for(IRPCDigiCollection::DigiRangeIterator detUnitIt = irpcDigis->begin(); detUnitIt != irpcDigis->end(); ++detUnitIt) {
	int totClu = 0;
	for (auto detUnitIt = irpcDigis->begin(); detUnitIt != irpcDigis->end(); ++detUnitIt) {
		auto detId = (*detUnitIt).first;
		auto digiRange = (*detUnitIt).second;
		//const auto *roll = rpcGeom.roll(detId);
		 auto roll = dynamic_cast<const RPCRoll*>(rpcGeom->roll(detId));
		if (roll==nullptr) {
			edm::LogError("BadDigiInput") << "Failed to find RPCRoll for ID " << detId;
			continue;
		}

    	std::cout << std::endl;
    	std::cout << "Debugging..." << std::endl;
    	std::cout << "rpcId: " << detId << std::endl;
    	std::cout << "roll: " << roll << std::endl;
    
    	// Get the IRPC Info
    	IRPCInfo info;
    	info.setThrTimeHR( thrTime );
    	info.setThrTimeLR( thrTime );
    	info.setThrStripNum( thrStripNum );

    	// Call the reconstruction algorithm
    	OwnVector<IRPCRecHit> recHits = theAlgo->reconstruct(*roll, detId, digiRange, info);

    	if (!recHits.empty()) {   //FIXME: is it really needed?
    	    recHitCollection->put(detId, recHits.begin(), recHits.end());
    	}
		totClu += recHits.size();
	}
	//h1_NClusters_event->Fill(totClu);
    iEvent.put(std::move(recHitCollection));
}



    //IRPCDigiCollection::DigiRangeIterator detUnitIt;
    //for (detUnitIt = rpcDigis->begin(); detUnitIt != rpcDigis->end(); ++detUnitIt) {
//	for (IRPCDigiCollection::DigiRangeIterator detUnitIt = irpcDigis->begin(); detUnitIt != irpcDigis->end(); ++detUnitIt) {
//
//	  //std::cout << " here 1" << std::endl;
//
//	  auto id = (*detUnitIt).first;
//	  auto digiRange =  (*detUnitIt).second;
//	  auto roll = dynamic_cast<const RPCRoll*>(rpcGeom->roll(id));
//
//	  if (roll==nullptr) {
//		  edm::LogError("BadDigiInput") << "Failed to find RPCRoll for ID " << id;
//		  continue;
//      }
//      edm::LogVerbatim("RPCDump") << "  * new detector";
//      edm::LogVerbatim("RPCDump") << "   - id: " << id.rawId() << "\n   - number of strip " << roll->nstrips();
//
//      // Loop over the digis of this DetUnit
//      //for (IRPCDigiCollection::const_iterator digiIt = digiRange.first; digiIt != digiRange.second; ++digiIt) {
//	  for (auto digi = digiRange.first; digi != digiRange.second; ++digi){
//	    int bunchX = digi->bx();
//		int strip = digi->strip();
////		float timeHR = abs(IRPCDigiTime( *digi ).timeHR());
////		float timeLR = abs(IRPCDigiTime( *digi ).timeLR());
//
//		// this occurs operator error
//        //edm::LogVerbatim("RPCDump") << "     strip: " << strip << " bx: " << bunchX << " tHR: " << timeHR << " tLR: " << timeLR;
//        edm::LogVerbatim("RPCDump") << "     strip: " << strip << " bx: " << bunchX;
//        if (digi->strip() < 1 || digi->strip() > roll->nstrips()) {
//          edm::LogVerbatim("RPCDump") << " XXXXXXXXXXXXX Problemt with " << id;
//        }
//	}
//  }
//
//}







//	edm::LogVerbatim("RPCDump") << "* new Run \n - Run: " << iEvent.id().run() << " \n - Event: " << iEvent.id().event();
//
//	std::cout << "run: " << iEvent.id().run() << " id: " << iEvent.id().event() << std::endl;
//
//    // Get the RPC Geometry
//	const auto& irpcDigis = iEvent.getHandle(irpcDigiToken);
//    const auto& rpcGeom = iSetup.getHandle(rpcGeomToken);
//
//    // Pass the EventSetup to the algo
//    theAlgo->setES(iSetup);
//
//
//    // Create the pointer to the collection which will store the rechits
//    auto recHitCollection = std::make_unique<IRPCRecHitCollection>();
//
//    // Iterate through all digi collections ordered by LayerId
//
//	//for(IRPCDigiCollection::DigiRangeIterator detUnitIt = irpcDigis->begin(); detUnitIt != irpcDigis->end(); ++detUnitIt) {
//	for (auto detUnitIt = irpcDigis->begin(); detUnitIt != irpcDigis->end(); ++detUnitIt) {
//		auto detId = (*detUnitIt).first;
//		auto digiRange = (*detUnitIt).second;
//		//const auto *roll = rpcGeom.roll(detId);
//		 auto roll = dynamic_cast<const RPCRoll*>(rpcGeom->roll(detId));
//		if (roll==nullptr) {
//			edm::LogError("BadDigiInput") << "Failed to find RPCRoll for ID " << detId;
//			continue;
//		}
//    
//    	// Get the IRPC Info
//    	IRPCInfo info;
//    	info.setThrTimeHR( thrTime );
//    	info.setThrTimeLR( thrTime );
//    	info.setThrStripNum( thrStripNum );
//
//    	// Call the reconstruction algorithm
//    	OwnVector<IRPCRecHit> recHits = theAlgo->reconstruct(*roll, detId, digiRange, info);
//
//    	if (!recHits.empty()) {   //FIXME: is it really needed?
//    	    recHitCollection->put(detId, recHits.begin(), recHits.end());
//    	}
//	}
//
//    iEvent.put(std::move(recHitCollection));
//		for (auto digi = digiRange.first; digi != digiRange.second; ++digi) {
//            // The layerId
//            //const RPCDetId& rpcId = (*rpcdgIt).first;
//    
//    		std::cout << std::endl;
//    		std::cout << "Debugging..." << std::endl;
//    		std::cout << "rpcId: " << rpcId << std::endl;
//    
//    		std::cout << "roll: " << rpcGeom.roll(rpcId) << std::endl;
//            // Get the GeomDet from the setup
//            const RPCRoll* roll = rpcGeom.roll(rpcId);
//            if (roll == nullptr) {
//                edm::LogError("BadDigiInput") << "Failed to find RPCRoll for ID " << rpcId;
//                continue;
//            }
//    
//            // Get the iterators over the digis associated with this LayerId
//            const IRPCDigiCollection::Range& range = (*rpcdgIt).second;
    
            // Getting the roll mask, that includes dead strips, for the given IRPCDet
    //        RollMask mask;
    //        const int rawId = rpcId.rawId();
    //        for (const auto& tomask : theIRPCMaskedStripsObj->MaskVec) {
    //            if (tomask.rawId == rawId) {
    //                const int bit = tomask.strip;
    //                mask.set(bit - 1);
    //            }
    //        }
    //
    //        for (const auto& tomask : theIRPCDeadStripsObj->DeadVec) {
    //            if (tomask.rawId == rawId) {
    //                const int bit = tomask.strip;
    //                mask.set(bit - 1);
    //            }
        
    
    	
    	

