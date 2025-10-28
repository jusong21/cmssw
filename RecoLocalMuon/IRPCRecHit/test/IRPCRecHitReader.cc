#include <memory>
#include <string>
#include <iostream>

#include "DataFormats/IRPCRecHit/interface/IRPCRecHit.h"
#include "DataFormats/IRPCRecHit/interface/IRPCRecHitCollection.h"

#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"

#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "TH1D.h"

class IRPCRecHitReader : public edm::one::EDAnalyzer<> {

	public:
		explicit IRPCRecHitReader(const edm::ParameterSet & conf);
		virtual ~IRPCRecHitReader();
	
		virtual void analyze(const edm::Event& e, const edm::EventSetup& c) override;
	
	private:
		// member data
		edm::EDGetTokenT<IRPCRecHitCollection> irpcRecHitToken;
		TH1D* h_test;
	
	#ifdef THIS_IS_AN_EVENTSETUP_EXAMPLE
		edm::ESGetToken<SetupData, SetupRecord> setupToken_;
	#endif
};

IRPCRecHitReader::IRPCRecHitReader(const edm::ParameterSet& iConfig) {
	#ifdef THIS_IS_AN_EVENTSETUP_EXAMPLE
		setupDataToken_ = esConsumes<SetupData, SetupRecord>();
	#endif

	irpcRecHitToken = consumes<IRPCRecHitCollection>(iConfig.getParameter<edm::InputTag>("irpcRecHitTag"));
	//usesResource("TFileService");
	edm::Service<TFileService> fs;

	h_test = fs->make<TH1D>("h_test", "h_test", 30, 0, 30);
	
	// 1. Timing Analysis
	h1_timeHR = fs->make<TH1D>("h1_timeHR", "High Radious Time;Time HR [ns];Entries", 100, -10, 10);
	h1_timeLR = fs->make<TH1D>("h1_timeLR", "Low Radious Time;Time LR [ns];Entries", 100, -10, 10);
	h1_deltaTime = fs->make<TH1D>("h1_deltaTime", "#DeltaT (HR-LR);#DeltaT = T_{HR} - T_{LR} [ns];Entries", 100, -10, 10);
	h2_timeHR_vs_timeLR = fs->make<TH2D>("h2_timeHR_vs_timeLR", "HR vs LR Time;Time LR [ns];Time HR [ns]", 100, -10, 10, 100, -10, 10);
	h1_timeHRErr = fs->make<TH1D>("h1_timeHRErr", "HR Time Error;Time HR Error [ns];Entries", 50, -.5, .5);
	h1_timeLRErr = fs->make<TH1D>("h1_timeLRErr", "LR Time Error;Time LR Error [ns];Entries", 50, -.5, .5);
	
	// 2. Position Analysis
	h1_x_local = fs->make<TH1D>("h1_x_local", "Local X Position;X_{local} [cm];Entries", 130, -60, 5);
	h1_y_local = fs->make<TH1D>("h1_y_local", "Local Y Position;Y_{local} [cm];Entries", 100, -.5, .5);
	h2_xy_local = fs->make<TH2D>("h2_xy_local", "Local XY Hit Map;X_{local} [cm];Y_{local} [cm]", 130, -60, 5, 50, -.5, .5);
	
	// 3. Cluster Properties
	h1_clusterSize = fs->make<TH1D>("h1_clusterSize", "Cluster Size;Cluster Size;Entries", 25, 0, 25);
	h1_firstStrip = fs->make<TH1D>("h1_firstStrip", "First Strip Number;Strip Number;Entries", 100, 0, 100);
	h2_deltaTime_vs_clusterSize = fs->make<TH2D>("h2_deltaTime_vs_clusterSize", "#DeltaT vs Cluster Size;Cluster Size;#DeltaT [ns]", 25, 0, 25, 100, -10, 10);
	h2_y_vs_clusterSize = fs->make<TH2D>("h2_y_vs_clusterSize", "Y vs Cluster Size;Cluster Size;Y_{local} [cm]", 25, 0, 25, 100, -.5, .5);
	
	// 4. Efficiency & Occupancy
	h1_nRecHits_per_event = fs->make<TH1D>("h1_nRecHits_per_event", "RecHits per Event;N RecHits;Entries", 25, 0, 25);
	h1_bx = fs->make<TH1D>("h1_bx", "Bunch Crossing;BX;Entries", 21, -10, 10);
	
	// 5. Physics Validation - KEY PLOT!
	h2_y_vs_deltaTime = fs->make<TH2D>("h2_y_vs_deltaTime", "Y Position vs #DeltaT (Linearity Check);#DeltaT = T_{HR} - T_{LR} [ns];Y_{local} [cm]", 100, -10, 10, 100, -.5, .5);
	h1_y_resolution = fs->make<TH1D>("h1_y_resolution", "Y Position Resolution;Y_{local} [cm];Entries", 100, -.5, .5);
}

void IRPCRecHitReader::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
	
	using namespace edm;

	edm::Handle<IRPCRecHitCollection> irpcRecHits;
	iEvent.getByToken(irpcRecHitToken, irpcRecHits);

	std::cout << "Events analyzed #Run " << iEvent.id().run() << "#Events: " << iEvent.id().event() << std::endl;

	int nHit = 0;

	//IRPCRecHitCollection::const_iterator recIt;

	for (auto recIt = irpcRecHits->begin(); recIt != irpcRecHits->end(); recIt++) {
		//RPCDetId id = (RPCDetId)(*recIt).rpcId();

		nHit++;
		
		// Get detector ID
		RPCDetId rpcId = (RPCDetId)(*recIt).rpcId();
		
		// === 1. Timing Information ===
		float timeHR = recIt->timeHR();
		float timeLR = recIt->timeLR();
		float timeHRErr = recIt->timeHRError();
		float timeLRErr = recIt->timeLRError();
		float deltaTime = timeHR - timeLR;
		
		h1_timeHR->Fill(timeHR);
		h1_timeLR->Fill(timeLR);
		h1_deltaTime->Fill(deltaTime);
		h2_timeHR_vs_timeLR->Fill(timeLR, timeHR);
		
		if (timeHRErr > 0) h1_timeHRErr->Fill(timeHRErr);
		if (timeLRErr > 0) h1_timeLRErr->Fill(timeLRErr);
		
		// === 2. Position Information ===
		LocalPoint localPos = recIt->localPosition();
		float x_local = localPos.x();
		float y_local = localPos.y();
		
		h1_x_local->Fill(x_local);
		h1_y_local->Fill(y_local);
		h2_xy_local->Fill(x_local, y_local);
		h1_y_resolution->Fill(y_local);
		
		// === 3. Cluster Properties ===
		int clusterSize = recIt->clusterSize();
		int firstStrip = recIt->firstClusterStrip();
		// Note: bx information might not be directly accessible from RecHit
		// int bx = recIt->bx();
		
		h1_clusterSize->Fill(clusterSize);
		h1_firstStrip->Fill(firstStrip);
		// h1_bx->Fill(bx);
		
		// Correlations
		h2_deltaTime_vs_clusterSize->Fill(clusterSize, deltaTime);
		h2_y_vs_clusterSize->Fill(clusterSize, y_local);
		
		// === 4. KEY PHYSICS PLOT: Y vs DeltaT ===
		// This should show linearity: Y = deltaT / (2 * speed)
		h2_y_vs_deltaTime->Fill(deltaTime, y_local);
		
		// Debug output for first few hits
		if (nHit <= 5) {
			std::cout << "  Hit #" << nHit 
			          << " - Station: " << rpcId.station()
			          << " Ring: " << rpcId.ring()
			          << " Layer: " << rpcId.layer()
			          << " | ClusterSize: " << clusterSize
			          << " | Strip: " << firstStrip << std::endl;
			          // << " | BX: " << bx << std::endl;
			std::cout << "       TimeHR: " << timeHR 
			          << " ns, TimeLR: " << timeLR 
			          << " ns, DeltaT: " << deltaTime << " ns" << std::endl;
			std::cout << "       Position: X=" << x_local 
			          << " cm, Y=" << y_local << " cm" << std::endl;
		}
	}
	std::cout << "nHits: " << nHit << std::endl;
	h_test->Fill(nHit);
}

IRPCRecHitReader::~IRPCRecHitReader() {
	std::cout << "done" << std::endl;
}

DEFINE_FWK_MODULE(IRPCRecHitReader);
	
