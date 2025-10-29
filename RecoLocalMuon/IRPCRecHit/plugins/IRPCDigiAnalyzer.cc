// -*- C++ -*-
//
// Package:		IRPC/IRPCDigiAnalyzer
// Class:			IRPCDigiAnalyzer
//
/**\class IRPCDigiAnalyzer IRPCDigiAnalyzer.cc IRPC/IRPCDigiAnalyzer/plugins/IRPCDigiAnalyzer.cc

 Description: Analyzer for IRPC digi data - analyzes raw digis and clustering performance

 Implementation:
		 Analyzes IRPC digi collections, performs clustering analysis,
		 and creates histograms for timing, position, and efficiency studies
*/
//
// Original Author:	Borislav Pavlov
//				 Created:	Thu, 18 Jan 2012 17:57:10 GMT
// Adapted by Juhee Song (Hanyang Univ, Vrije Universiteit Brussel)
//

// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/RPCDigi/interface/IRPCDigi.h"
#include "DataFormats/RPCDigi/interface/IRPCDigiCollection.h"
#include "Geometry/RPCGeometry/interface/RPCRoll.h"
#include "DataFormats/MuonDetId/interface/RPCDetId.h"

#include "DataFormats/RPCDigi/interface/IRPCDigiTime.h"
#include "RecoLocalMuon/IRPCRecHit/interface/IRPCHit.h"
#include "RecoLocalMuon/IRPCRecHit/interface/IRPCHitContainer.h"
#include "RecoLocalMuon/IRPCRecHit/interface/IRPCCluster.h"
#include "RecoLocalMuon/IRPCRecHit/interface/IRPCClusterizer.h"
#include "RecoLocalMuon/IRPCRecHit/interface/IRPCInfo.h"

#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "TH1D.h"
#include "TH2D.h"
#include <iostream>

//
// class declaration
//
class IRPCClusterizer;

// If the analyzer does not use TFileService, please remove
// the template argument to the base class so the class inherits
// from	edm::one::EDAnalyzer<>
// This will improve performance in multithreaded jobs.

class IRPCDigiAnalyzer : public edm::one::EDAnalyzer<edm::one::SharedResources> {
public:
	explicit IRPCDigiAnalyzer(const edm::ParameterSet&);
	~IRPCDigiAnalyzer() override;

	static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
	void beginJob() override;
	void analyze(const edm::Event&, const edm::EventSetup&) override;
	void endJob() override;

	// ----------member data ---------------------------
	edm::EDGetTokenT<IRPCDigiCollection> irpcDigiToken;
	double thrTime;
	double thrStripNum;

	// Timing analysis histograms
	TH1D* h1_timeHR;
	TH1D* h1_timeLR;
	TH1D* h1_deltaTime;
	TH2D* h2_timeHR_vs_timeLR;
	TH2D* h2_deltaTime_vs_strip;
	
	// Position analysis histograms
	TH1D* h1_stripNumber;
	TH1D* h1_bunchCrossing;
	TH2D* h2_strip_vs_timeHR;
	TH2D* h2_strip_vs_timeLR;
	
	// Cluster analysis histograms
	TH1D* h1_nDigis_per_event;
	TH1D* h1_nDigis_per_detector;
	TH1D* h1_nClusters_per_event;
	TH1D* h1_nClusters_per_detector;
	TH1D* h1_nClustersHR_per_event;
	TH1D* h1_nClustersLR_per_event;
	TH1D* h1_nClustersHR_per_detector;
	TH1D* h1_nClustersLR_per_detector;
	TH2D* h2_nDigis_vs_nClusters;
	TH2D* h2_clustersHR_vs_clustersLR;
	
	// Efficiency and occupancy
	TH1D* h1_nDetectors_per_event;
	TH1D* h1_clusters_per_detector_ratio;
	TH2D* h2_nDetectors_vs_nClusters;
	TH2D* h2_occupancy_station_sector;
	TH2D* h2_occupancy_station_layer;
	TH2D* h2_occupancy_ring_sector;
	TH2D* h2_bunchCrossing_vs_station;
	
	// Cluster properties
	TH1D* h1_clusterSize;
	TH1D* h1_firstStrip;
	TH2D* h2_clusterSize_vs_deltaTime;
	TH2D* h2_firstStrip_vs_clusterSize;

#ifdef THIS_IS_AN_EVENTSETUP_EXAMPLE
	edm::ESGetToken<SetupData, SetupRecord> setupToken_;
#endif
};


IRPCDigiAnalyzer::~IRPCDigiAnalyzer() {
}

IRPCDigiAnalyzer::IRPCDigiAnalyzer(const edm::ParameterSet& iConfig)
{
	//now do what ever initialization is needed
	irpcDigiToken = consumes<IRPCDigiCollection>(
								 iConfig.getParameter<edm::InputTag>("irpcDigiTag"));
	thrTime = iConfig.getParameter<double>("thrTime");
	thrStripNum = iConfig.getParameter<double>("thrStripNum");

	usesResource("TFileService");
	edm::Service<TFileService> fs;

	// Timing analysis histograms
	h1_timeHR = fs->make<TH1D>("h1_timeHR", "High Radius Time;T_{HR} [ns];Entries", 100, -10, 10);
	h1_timeLR = fs->make<TH1D>("h1_timeLR", "Low Radius Time;T_{LR} [ns];Entries", 100, -10, 10);
	h1_deltaTime = fs->make<TH1D>("h1_deltaTime", "Time Difference;#DeltaT = T_{HR} - T_{LR} [ns];Entries", 100, -10, 10);
	h2_timeHR_vs_timeLR = fs->make<TH2D>("h2_timeHR_vs_timeLR", "T_{HR} vs T_{LR};T_{LR} [ns];T_{HR} [ns]", 100, -10, 10, 100, -10, 10);
	h2_deltaTime_vs_strip = fs->make<TH2D>("h2_deltaTime_vs_strip", "#DeltaT vs Strip Number;Strip Number;#DeltaT [ns]", 100, 0, 100, 100, -10, 10);
	
	// Position analysis histograms
	h1_stripNumber = fs->make<TH1D>("h1_stripNumber", "Strip Number Distribution;Strip Number;Entries", 100, 0, 100);
	h1_bunchCrossing = fs->make<TH1D>("h1_bunchCrossing", "Bunch Crossing Distribution;Bunch Crossing;Entries", 4, -2, 2);
	h2_strip_vs_timeHR = fs->make<TH2D>("h2_strip_vs_timeHR", "Strip vs T_{HR};Strip Number;T_{HR} [ns]", 100, 0, 100, 100, -10, 10);
	h2_strip_vs_timeLR = fs->make<TH2D>("h2_strip_vs_timeLR", "Strip vs T_{LR};Strip Number;T_{LR} [ns]", 100, 0, 100, 100, -10, 10);
	
	// Cluster analysis histograms
	h1_nDigis_per_event = fs->make<TH1D>("h1_nDigis_per_event", "Number of Digis per Event;N_{digis};Events", 50, 0, 50);
	h1_nDigis_per_detector = fs->make<TH1D>("h1_nDigis_per_detector", "Number of Digis per Detector;N_{digis};Detectors", 50, 0, 50);
	h1_nClusters_per_event = fs->make<TH1D>("h1_nClusters_per_event", "Number of Clusters per Event;N_{clusters};Events", 25, 0, 25);
	h1_nClusters_per_detector = fs->make<TH1D>("h1_nClusters_per_detector", "Number of Clusters per Detector;N_{clusters};Detectors", 25, 0, 25);
	h1_nClustersHR_per_event = fs->make<TH1D>("h1_nClustersHR_per_event", "Number of HR Clusters per Event;N_{clusters} (HR);Events", 25, 0, 25);
	h1_nClustersLR_per_event = fs->make<TH1D>("h1_nClustersLR_per_event", "Number of LR Clusters per Event;N_{clusters} (LR);Events", 25, 0, 25);
	h1_nClustersHR_per_detector = fs->make<TH1D>("h1_nClustersHR_per_detector", "Number of HR Clusters per Detector;N_{clusters} (HR);Detectors", 25, 0, 25);
	h1_nClustersLR_per_detector = fs->make<TH1D>("h1_nClustersLR_per_detector", "Number of LR Clusters per Detector;N_{clusters} (LR);Detectors", 25, 0, 25);
	h2_nDigis_vs_nClusters = fs->make<TH2D>("h2_nDigis_vs_nClusters", "N_{digis} vs N_{clusters};N_{digis};N_{clusters}", 50, 0, 50, 25, 0, 25);
	h2_clustersHR_vs_clustersLR = fs->make<TH2D>("h2_clustersHR_vs_clustersLR", "HR vs LR Clusters per Event;N_{clusters} (LR);N_{clusters} (HR)", 24, 1, 25, 24, 1, 25);
	
	// Efficiency and occupancy
	h1_nDetectors_per_event = fs->make<TH1D>("h1_nDetectors_per_event", "Number of Detectors per Event;N_{detectors};Events", 7, 0, 7);
	h1_clusters_per_detector_ratio = fs->make<TH1D>("h1_clusters_per_detector_ratio", "Clusters per Detector Ratio;N_{clusters} / N_{detectors};Events", 50, 0, 5);
	h2_nDetectors_vs_nClusters = fs->make<TH2D>("h2_nDetectors_vs_nClusters", "Detectors vs Clusters per Event;N_{detectors};N_{clusters}", 7, 0, 7, 25, 0, 25);
	h2_occupancy_station_sector = fs->make<TH2D>("h2_occupancy_station_sector", "Occupancy by Station and Sector;Sector;Station", 12, 1, 13, 4, 1, 5);
	h2_occupancy_station_layer = fs->make<TH2D>("h2_occupancy_station_layer", "Occupancy by Station and Layer;Layer;Station", 2, 1, 3, 4, 1, 5);
	h2_occupancy_ring_sector = fs->make<TH2D>("h2_occupancy_ring_sector", "Occupancy by Ring and Sector;Sector;Ring", 12, 1, 13, 3, 1, 4);
	h2_bunchCrossing_vs_station = fs->make<TH2D>("h2_bunchCrossing_vs_station", "Bunch Crossing vs Station;Station;Bunch Crossing", 4, 1, 5, 21, -10, 10);
	
	// Cluster properties
	h1_clusterSize = fs->make<TH1D>("h1_clusterSize", "Cluster Size Distribution;Cluster Size;Entries", 25, 0, 25);
	h1_firstStrip = fs->make<TH1D>("h1_firstStrip", "First Strip Distribution;First Strip;Entries", 100, 0, 100);
	h2_clusterSize_vs_deltaTime = fs->make<TH2D>("h2_clusterSize_vs_deltaTime", "Cluster Size vs #DeltaT;Cluster Size;#DeltaT [ns]", 25, 0, 25, 100, -10, 10);
	h2_firstStrip_vs_clusterSize = fs->make<TH2D>("h2_firstStrip_vs_clusterSize", "First Strip vs Cluster Size;Cluster Size;First Strip", 25, 0, 25, 100, 0, 100);
}


//
// member functions
//

// ------------ method called for each event	------------
void IRPCDigiAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
	using namespace edm;

	// Get the RPC digis from the event																																																		 
	Handle<IRPCDigiCollection> irpcDigis;
	iEvent.getByToken(irpcDigiToken, irpcDigis);
	
	// Event-level counters
	int totalDigis = 0;
	int totalClusters = 0;
	int totalClustersHR = 0;
	int totalClustersLR = 0;
	int nDetectors = 0;
	
	// Process each detector
	for(IRPCDigiCollection::DigiRangeIterator detUnitIt = irpcDigis->begin(); detUnitIt != irpcDigis->end(); ++detUnitIt) {
		nDetectors++;
		
		RPCDetId detId = (*detUnitIt).first; // RPCDetId
		auto digiRange = (*detUnitIt).second;

		// Extract detector geometry information
		int region = detId.region();    // 0 = Barrel, +/-1 = Endcap
		int ring = detId.ring();        // Ring number
		int station = detId.station();  // Station number (1-4)
		int sector = detId.sector();    // Sector number
		int layer = detId.layer();      // Layer number
		int subsector = detId.subsector(); // Subsector
		int roll = detId.roll();        // Roll number
		
		// Detector-level counters
		int nDigisPerDet = 0;
		int nClustersPerDet = 0;
		int nClustersHRPerDet = 0;
		int nClustersLRPerDet = 0;
		
		// Set up clustering parameters
		IRPCInfo info;
		info.setThrTimeHR(thrTime);
		info.setThrTimeLR(thrTime);
		info.setThrStripNum(thrStripNum);
		
		// Organize digis by bunch crossing
		std::map<int, std::pair<IRPCHitContainer, IRPCHitContainer>> hits; // <bx, <hr_hits, lr_hits>>
		
		// Process individual digis
		for (auto digi = digiRange.first; digi != digiRange.second; ++digi) {
			nDigisPerDet++;
			totalDigis++;
			
			int bunchX = digi->bx();
			int strip = digi->strip();
			float timeHR = IRPCDigiTime(*digi).timeHR();
			float timeLR = IRPCDigiTime(*digi).timeLR();
			float deltaTime = timeHR - timeLR;
			
			// Fill timing histograms
			h1_timeHR->Fill(timeHR);
			h1_timeLR->Fill(timeLR);
			h1_deltaTime->Fill(deltaTime);
			h2_timeHR_vs_timeLR->Fill(timeLR, timeHR);
			h2_deltaTime_vs_strip->Fill(strip, deltaTime);
			
			// Fill position histograms
			h1_stripNumber->Fill(strip);
			h1_bunchCrossing->Fill(bunchX);
			h2_strip_vs_timeHR->Fill(strip, timeHR);
			h2_strip_vs_timeLR->Fill(strip, timeLR);
			
			// Fill detector occupancy using physical detector structure
			h2_occupancy_station_sector->Fill(sector, station);
			h2_occupancy_station_layer->Fill(layer, station);
			h2_occupancy_ring_sector->Fill(sector, ring);
			h2_bunchCrossing_vs_station->Fill(station, bunchX);
			
			// Create hits for clustering
			if (hits.find(bunchX) == hits.end()) {
				hits[bunchX] = std::make_pair(IRPCHitContainer(), IRPCHitContainer());
			}
			
			// Add HR hit
			hits[bunchX].first.push_back(IRPCHit(strip, timeHR, bunchX));
			hits[bunchX].first.back().setHR(true);
			
			// Add LR hit
			hits[bunchX].second.push_back(IRPCHit(strip, timeLR, bunchX));
			hits[bunchX].second.back().setLR(true);
		}
		
		// Perform clustering analysis
		IRPCClusterizer clusterizer;
		IRPCClusterContainer allClusters;
		
		for (auto& [bx, hitCont] : hits) {
			IRPCClusterContainer clustersHR, clustersLR, finalClusters;
			
			// One-side clustering
			clusterizer.oneSideClusterizer(info.thrTimeHR(), hitCont.first, clustersHR);
			clusterizer.oneSideClusterizer(info.thrTimeLR(), hitCont.second, clustersLR);
			
			// Count one-side clusters (event and detector level)
			int nHR = clustersHR.size();
			int nLR = clustersLR.size();
			
			// Debug: Print cluster counts and analyze differences
			if (nHR != nLR) {
				std::cout << "  *** MISMATCH *** HR clusters: " << nHR << ", LR clusters: " << nLR << std::endl;
				std::cout << "  HR hit times: ";
				for(const auto& hit : hitCont.first) {
					std::cout << hit.time() << " ";
				}
				std::cout << std::endl;
				std::cout << "  LR hit times: ";
				for(const auto& hit : hitCont.second) {
					std::cout << hit.time() << " ";
				}
				std::cout << std::endl;
				std::cout << "  Cluster sizes - HR: ";
				for(const auto& cl : clustersHR) {
					std::cout << cl.nStrip() << " ";
					std::cout << cl.firstStrip() << " ";
					std::cout << cl.lastStrip() << " ";
				}
				std::cout << "  first sizes - HR: ";
				for(const auto& cl : clustersHR) {
					std::cout << cl.firstStrip() << " ";
				}
				std::cout << std::endl;
				std::cout << "  Cluster sizes - LR: ";
				for(const auto& cl : clustersLR) {
					std::cout << cl.nStrip() << " ";
					std::cout << cl.firstStrip() << " ";
					std::cout << cl.lastStrip() << " ";
				}
				std::cout << std::endl;
			}
			
			totalClustersHR += nHR;
			totalClustersLR += nLR;
			nClustersHRPerDet += nHR;
			nClustersLRPerDet += nLR;
			
			// Final clustering
			if (!clustersHR.empty() && !clustersLR.empty()) {
				finalClusters = clusterizer.finalClusterizer(clustersHR, clustersLR, info.thrStripNum());
			}

			// Process final clusters
			for (auto& cluster : finalClusters) {
				cluster.compute(info);
				nClustersPerDet++;
				totalClusters++;
				
				// Fill cluster property histograms
				h1_clusterSize->Fill(cluster.clusterSize());
				h1_firstStrip->Fill(cluster.firstStrip());
				
				float clusterDeltaTime = cluster.highTime() - cluster.lowTime();
				h2_clusterSize_vs_deltaTime->Fill(cluster.clusterSize(), clusterDeltaTime);
				h2_firstStrip_vs_clusterSize->Fill(cluster.clusterSize(), cluster.firstStrip());
			}
		}
		
		// Fill detector-level histograms
		h1_nDigis_per_detector->Fill(nDigisPerDet);
		h1_nClusters_per_detector->Fill(nClustersPerDet);
		h1_nClustersHR_per_detector->Fill(nClustersHRPerDet);
		h1_nClustersLR_per_detector->Fill(nClustersLRPerDet);
		h2_nDigis_vs_nClusters->Fill(nDigisPerDet, nClustersPerDet);
	}
	
	// Fill event-level histograms
	h1_nDigis_per_event->Fill(totalDigis);
	h1_nClusters_per_event->Fill(totalClusters);
	h1_nClustersHR_per_event->Fill(totalClustersHR);
	h1_nClustersLR_per_event->Fill(totalClustersLR);
	h2_clustersHR_vs_clustersLR->Fill(totalClustersLR, totalClustersHR);
	h1_nDetectors_per_event->Fill(nDetectors);
	h2_nDetectors_vs_nClusters->Fill(nDetectors, totalClusters);
	
	// Calculate and fill ratio
	if (nDetectors > 0) {
		double ratio = (double)totalClusters / nDetectors;
		h1_clusters_per_detector_ratio->Fill(ratio);
	}
}

// ------------ method called once each job just before starting event loop	------------
void IRPCDigiAnalyzer::beginJob() {
	// please remove this method if not needed
}

// ------------ method called once each job just after ending the event loop	------------
void IRPCDigiAnalyzer::endJob() {
	// please remove this method if not needed
}

// ------------ method fills 'descriptions' with the allowed parameters for the module	------------
void IRPCDigiAnalyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
	//The following says we do not know what parameters are allowed so do no validation
	// Please change this to state exactly what you do use, even if it is no parameters
	edm::ParameterSetDescription desc;
	desc.setUnknown();
	descriptions.addDefault(desc);

	//Specify that only 'tracks' is allowed
	//To use, remove the default given above and uncomment below
	//ParameterSetDescription desc;
	//desc.addUntracked<edm::InputTag>("tracks","ctfWithMaterialTracks");
	//descriptions.addWithDefaultLabel(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(IRPCDigiAnalyzer);
