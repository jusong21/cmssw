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


void IRPCRecHitProducer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup) {


//	std::cout << std::endl; 
//	std::cout << "*************************" << std::endl;
//    edm::LogVerbatim("RPCDump") << "**** new Run \n - Run: " << iEvent.id().run() << " \n - Event: " << iEvent.id().event();

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

//    	std::cout << std::endl;
//    	std::cout << "Debugging..." << std::endl;
//    	std::cout << "rpcId: " << detId << std::endl;
//    	std::cout << "roll: " << roll << std::endl;
    
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

