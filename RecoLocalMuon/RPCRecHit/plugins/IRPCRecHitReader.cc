#include <memory>
#include <string>
#include <iostream>
#include <map>
#include <sstream>

#include "DataFormats/RPCRecHit/interface/IRPCRecHit.h"
#include "DataFormats/RPCRecHit/interface/IRPCRecHitCollection.h"
#include "DataFormats/RPCDigi/interface/IRPCDigiCollection.h"
#include "DataFormats/RPCDigi/interface/IRPCDigiTime.h"

#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"

#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "Geometry/RPCGeometry/interface/RPCGeometry.h"
#include "Geometry/Records/interface/MuonGeometryRecord.h"

#include "RecoLocalMuon/RPCRecHit/interface/IRPCClusterizer.h"
#include "RecoLocalMuon/RPCRecHit/interface/IRPCInfo.h"
#include "RecoLocalMuon/RPCRecHit/interface/IRPCHit.h"
#include "RecoLocalMuon/RPCRecHit/interface/IRPCHitContainer.h"
#include "RecoLocalMuon/RPCRecHit/interface/IRPCClusterContainer.h"

#include "TH1D.h"
#include "TH2D.h"

class IRPCRecHitReader : public edm::one::EDAnalyzer<> {

	public:
		explicit IRPCRecHitReader(const edm::ParameterSet & conf);
		virtual ~IRPCRecHitReader();
	
		virtual void analyze(const edm::Event& e, const edm::EventSetup& c) override;
	
	private:
		// member data
		edm::EDGetTokenT<IRPCRecHitCollection> irpcRecHitToken;
		edm::EDGetTokenT<IRPCDigiCollection> irpcDigiToken;
		edm::ESGetToken<RPCGeometry, MuonGeometryRecord> rpcGeomToken;
		bool validateClustering_{false};
		double thrTime_{0.0};
		double thrStripNum_{0.0};

		// 1. Timing Analysis
		TH1D* h1_timeHR;
		TH1D* h1_timeLR;
		TH1D* h1_deltaTime;
		TH2D* h2_timeHR_vs_timeLR;
		
		// 2. Position Analysis
		TH1D* h1_x_local;
		TH1D* h1_y_local;
		TH2D* h2_xy_local;
		TH1D* h1_x_global;
		TH1D* h1_y_global;
		TH2D* h2_xy_global;
		std::map<std::string, TH2D*> h2_xy_local_by_roll_;
		std::map<std::string, TH2D*> h2_xy_global_by_roll_;
		
		// 3. Cluster Properties
		TH1D* h1_clusterSize;
		TH1D* h1_firstStrip;
		TH2D* h2_deltaTime_vs_clusterSize;
		TH2D* h2_y_vs_clusterSize;
		
		// 4. Efficiency & Occupancy
		TH1D* h1_nRecHits_per_event;
		TH1D* h1_bx;
		
		// 5. Physics Validation - KEY PLOT!
		TH2D* h2_y_vs_deltaTime;
		TH1D* h1_y_resolution;

		// Optional clustering validation: expected clusters - rechits
		TH1D* h_nClusterDiff_{nullptr};
		TH1D* h_nHRClustersPerDet_{nullptr};
		TH1D* h_nLRClustersPerDet_{nullptr};
		TH1D* h_nFinalClustersPerDet_{nullptr};
		TH1D* h_nUnmatchedHRPerDet_{nullptr};
		TH1D* h_nUnmatchedLRPerDet_{nullptr};
		TH1D* h_deltaStripMatched_{nullptr};
		TH2D* h2_y_vs_deltaTime_cluster_{nullptr};
	
	#ifdef THIS_IS_AN_EVENTSETUP_EXAMPLE
		edm::ESGetToken<SetupData, SetupRecord> setupToken_;
	#endif
};

IRPCRecHitReader::IRPCRecHitReader(const edm::ParameterSet& iConfig) {
	#ifdef THIS_IS_AN_EVENTSETUP_EXAMPLE
		setupDataToken_ = esConsumes<SetupData, SetupRecord>();
	#endif

	irpcRecHitToken = consumes<IRPCRecHitCollection>(iConfig.getParameter<edm::InputTag>("irpcRecHitTag"));
	rpcGeomToken = esConsumes<RPCGeometry, MuonGeometryRecord>();
	validateClustering_ = iConfig.existsAs<bool>("validateClustering") ? iConfig.getParameter<bool>("validateClustering") : false;
	if (validateClustering_) {
		irpcDigiToken = consumes<IRPCDigiCollection>(iConfig.getParameter<edm::InputTag>("irpcDigiTag"));
		thrTime_ = iConfig.getParameter<double>("thrTime");
		thrStripNum_ = iConfig.getParameter<double>("thrStripNum");
	}
	//usesResource("TFileService");
	edm::Service<TFileService> fs;
	
	// 1. Timing Analysis
	h1_timeHR = fs->make<TH1D>("h1_timeHR", "High Radious Time;Time HR [ns];Entries", 100, -10, 10);
	h1_timeLR = fs->make<TH1D>("h1_timeLR", "Low Radious Time;Time LR [ns];Entries", 100, -10, 10);
	h1_deltaTime = fs->make<TH1D>("h1_deltaTime", "#DeltaT (HR-LR);#DeltaT = T_{HR} - T_{LR} [ns];Entries", 100, -10, 10);
	h2_timeHR_vs_timeLR = fs->make<TH2D>("h2_timeHR_vs_timeLR", "HR vs LR Time;Time LR [ns];Time HR [ns]", 100, -10, 10, 100, -10, 10);
	
	// 2. Position Analysis
	h1_x_local = fs->make<TH1D>("h1_x_local", "Local X Position;X_{local} [cm];Entries", 130, -60, 5);
	h1_y_local = fs->make<TH1D>("h1_y_local", "Local Y Position;Y_{local} [cm];Entries", 100, -.5, .5);
	h2_xy_local = fs->make<TH2D>("h2_xy_local", "Local XY Hit Map;X_{local} [cm];Y_{local} [cm]", 130, -60, 5, 50, -.5, .5);
	// Global XY zoomed for typical iRPC endcap ring (r ~ few m); CMSSW GlobalPoint is in cm.
	h1_x_global = fs->make<TH1D>("h1_x_global", "Global X Position;X_{global} [cm];Entries", 200, -450, 450);
	h1_y_global = fs->make<TH1D>("h1_y_global", "Global Y Position;Y_{global} [cm];Entries", 200, -450, 450);
	h2_xy_global = fs->make<TH2D>("h2_xy_global", "Global XY Hit Map;X_{global} [cm];Y_{global} [cm]", 200, -450, 450, 200, -450, 450);
	
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

	if (validateClustering_) {
		h_nClusterDiff_ = fs->make<TH1D>("h_nClusterDiff",
		                                  "Expected clusters - Rechits;N_{exp}-N_{rechit};Entries",
		                                  51, -25, 26);
		h_nHRClustersPerDet_ = fs->make<TH1D>("h_nHRClustersPerDet",
		                                      "HR one-side clusters per detector;N_{HR clusters};Entries",
		                                      30, 0, 30);
		h_nLRClustersPerDet_ = fs->make<TH1D>("h_nLRClustersPerDet",
		                                      "LR one-side clusters per detector;N_{LR clusters};Entries",
		                                      30, 0, 30);
		h_nFinalClustersPerDet_ = fs->make<TH1D>("h_nFinalClustersPerDet",
		                                         "Final clusters per detector;N_{final clusters};Entries",
		                                         30, 0, 30);
		h_nUnmatchedHRPerDet_ = fs->make<TH1D>("h_nUnmatchedHRPerDet",
		                                       "Unmatched HR clusters per detector;N_{unmatched HR};Entries",
		                                       30, 0, 30);
		h_nUnmatchedLRPerDet_ = fs->make<TH1D>("h_nUnmatchedLRPerDet",
		                                       "Unmatched LR clusters per detector;N_{unmatched LR};Entries",
		                                       30, 0, 30);
		h_deltaStripMatched_ = fs->make<TH1D>("h_deltaStripMatched",
		                                      "Matched #DeltaStrip (greedy 1:1);|#bar{s}_{HR}-#bar{s}_{LR}|;Pairs",
		                                      100, 0, 5);
		h2_y_vs_deltaTime_cluster_ = fs->make<TH2D>("h2_y_vs_deltaTime_cluster",
		                                            "Cluster-level Y vs #DeltaT;#DeltaT [ns];Y_{cluster}",
		                                            120, -6, 6, 120, -0.6, 0.6);
	}
}

void IRPCRecHitReader::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
	
	using namespace edm;

	edm::Handle<IRPCRecHitCollection> irpcRecHits;
	iEvent.getByToken(irpcRecHitToken, irpcRecHits);
	const auto& rpcGeom = iSetup.getHandle(rpcGeomToken);

	std::cout << "Events analyzed #Run " << iEvent.id().run() << "#Events: " << iEvent.id().event() << std::endl;

	int nHit = 0;
	std::map<unsigned int, int> actualCountByDet;
	if (validateClustering_) {
		actualCountByDet.clear();
	}

	//IRPCRecHitCollection::const_iterator recIt;

	for (auto recIt = irpcRecHits->begin(); recIt != irpcRecHits->end(); recIt++) {
		//RPCDetId id = (RPCDetId)(*recIt).rpcId();

		nHit++;
		
		// Get detector ID
		RPCDetId rpcId = (RPCDetId)(*recIt).rpcId();
		if (validateClustering_) {
			actualCountByDet[rpcId.rawId()]++;
		}
		
		// === 1. Timing Information ===
		float timeHR = recIt->timeHR();
		float timeLR = recIt->timeLR();
		float deltaTime = timeHR - timeLR;
		
		h1_timeHR->Fill(timeHR);
		h1_timeLR->Fill(timeLR);
		h1_deltaTime->Fill(deltaTime);
		h2_timeHR_vs_timeLR->Fill(timeLR, timeHR);
		
		// === 2. Position Information ===
		LocalPoint localPos = recIt->localPosition();
		float x_local = localPos.x();
		float y_local = localPos.y();
		
		h1_x_local->Fill(x_local);
		h1_y_local->Fill(y_local);
		h2_xy_local->Fill(x_local, y_local);
		h1_y_resolution->Fill(y_local);

		std::ostringstream detTag;
		detTag << "st" << rpcId.station() << "_rg" << rpcId.ring() << "_rl" << rpcId.roll();
		const std::string detKey = detTag.str();
		TH2D*& hLocalDet = h2_xy_local_by_roll_[detKey];
		if (hLocalDet == nullptr) {
			edm::Service<TFileService> fs;
			const std::string hName = "h2_xy_local_" + detKey;
			const std::string hTitle =
			    "Local XY Hit Map (" + detKey + ");X_{local} [cm];Y_{local} [cm]";
			hLocalDet = fs->make<TH2D>(hName.c_str(), hTitle.c_str(), 130, -60, 5, 50, -.5, .5);
		}
		hLocalDet->Fill(x_local, y_local);

		const RPCRoll* roll = dynamic_cast<const RPCRoll*>(rpcGeom->roll(rpcId));
		if (roll != nullptr) {
			const GlobalPoint globalPos = roll->toGlobal(localPos);
			h1_x_global->Fill(globalPos.x());
			h1_y_global->Fill(globalPos.y());
			h2_xy_global->Fill(globalPos.x(), globalPos.y());

			TH2D*& hGlobalDet = h2_xy_global_by_roll_[detKey];
			if (hGlobalDet == nullptr) {
				edm::Service<TFileService> fs;
				const std::string hName = "h2_xy_global_" + detKey;
				const std::string hTitle =
				    "Global XY Hit Map (" + detKey + ");X_{global} [cm];Y_{global} [cm]";
				hGlobalDet = fs->make<TH2D>(hName.c_str(), hTitle.c_str(), 200, -450, 450, 200, -450, 450);
			}
			hGlobalDet->Fill(globalPos.x(), globalPos.y());
		}
		
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
	h1_nRecHits_per_event->Fill(nHit);

	// Optional validation: compare expected final clusters (from digis) vs rechits.
	if (validateClustering_ && h_nClusterDiff_) {
		edm::Handle<IRPCDigiCollection> irpcDigis;
		iEvent.getByToken(irpcDigiToken, irpcDigis);

		IRPCInfo info;
		info.setThrTimeHR(thrTime_);
		info.setThrTimeLR(thrTime_);
		info.setThrStripNum(thrStripNum_);

		struct Candidate {
			size_t iHR;
			size_t iLR;
			float deltaStrip;
		};

		std::map<unsigned int, int> expectedCountByDet;
		for (auto detUnitIt = irpcDigis->begin(); detUnitIt != irpcDigis->end(); ++detUnitIt) {
			const auto detId = (*detUnitIt).first;
			const auto& digiRange = (*detUnitIt).second;

			// Build hit containers by BX and side.
			std::map<int, std::pair<IRPCHitContainer, IRPCHitContainer>> hitsByBx;
			for (auto digi = digiRange.first; digi != digiRange.second; ++digi) {
				const int bx = digi->bx();
				const int strip = digi->strip();
				const float tHR = IRPCDigiTime(*digi).timeHR();
				const float tLR = IRPCDigiTime(*digi).timeLR();
				h1_bx->Fill(bx);

				auto& bxEntry = hitsByBx[bx];
				bxEntry.first.push_back(IRPCHit(strip, tHR, bx));
				bxEntry.first.back().setHR(true);
				bxEntry.second.push_back(IRPCHit(strip, tLR, bx));
				bxEntry.second.back().setLR(true);
			}

			int nFinalDet = 0;
			for (auto& [bx, hitCont] : hitsByBx) {
				IRPCClusterizer clizer;
				IRPCClusterContainer clustersHR, clustersLR;
				(void)clizer.oneSideClusterizer(info.thrTimeHR(), hitCont.first, clustersHR);
				(void)clizer.oneSideClusterizer(info.thrTimeLR(), hitCont.second, clustersLR);

				if (h_nHRClustersPerDet_) h_nHRClustersPerDet_->Fill(static_cast<int>(clustersHR.size()));
				if (h_nLRClustersPerDet_) h_nLRClustersPerDet_->Fill(static_cast<int>(clustersLR.size()));

				// Reproduce greedy 1:1 matching used in finalClusterizer for diagnostics.
				std::vector<Candidate> candidates;
				candidates.reserve(clustersHR.size() * clustersLR.size());
				for (size_t iHR = 0; iHR < clustersHR.size(); ++iHR) {
					for (size_t iLR = 0; iLR < clustersLR.size(); ++iLR) {
						const float dS = std::abs(clustersHR[iHR].stripNumAvg() - clustersLR[iLR].stripNumAvg());
						if (dS < info.thrStripNum()) {
							candidates.push_back(Candidate{iHR, iLR, dS});
						}
					}
				}
				std::sort(candidates.begin(), candidates.end(),
				          [](const Candidate& a, const Candidate& b) {
					          if (a.deltaStrip != b.deltaStrip) return a.deltaStrip < b.deltaStrip;
					          if (a.iHR != b.iHR) return a.iHR < b.iHR;
					          return a.iLR < b.iLR;
				          });

				std::vector<char> usedHR(clustersHR.size(), 0);
				std::vector<char> usedLR(clustersLR.size(), 0);
				int nFinalBx = 0;
				for (const auto& cand : candidates) {
					if (usedHR[cand.iHR] || usedLR[cand.iLR]) continue;
					usedHR[cand.iHR] = 1;
					usedLR[cand.iLR] = 1;
					++nFinalBx;
					if (h_deltaStripMatched_) h_deltaStripMatched_->Fill(cand.deltaStrip);
				}

				const int nUnmatchedHR = static_cast<int>(clustersHR.size()) - nFinalBx;
				const int nUnmatchedLR = static_cast<int>(clustersLR.size()) - nFinalBx;
				if (h_nFinalClustersPerDet_) h_nFinalClustersPerDet_->Fill(nFinalBx);
				if (h_nUnmatchedHRPerDet_) h_nUnmatchedHRPerDet_->Fill(std::max(0, nUnmatchedHR));
				if (h_nUnmatchedLRPerDet_) h_nUnmatchedLRPerDet_->Fill(std::max(0, nUnmatchedLR));

				// Cluster-level Y vs DeltaT
				auto finalClusters = clizer.finalClusterizer(clustersHR, clustersLR, info.thrStripNum());
				for (auto& cl : finalClusters) {
					cl.compute(info);
					if (cl.hasDeltaTime() && cl.hasY() && h2_y_vs_deltaTime_cluster_) {
						h2_y_vs_deltaTime_cluster_->Fill(cl.deltaTime(), cl.y());
					}
				}

				nFinalDet += nFinalBx;
			}

			expectedCountByDet[detId.rawId()] = nFinalDet;
		}

		for (const auto& [rawId, nExp] : expectedCountByDet) {
			const int nAct = actualCountByDet[rawId]; // 0 if missing
			h_nClusterDiff_->Fill(nExp - nAct);
		}
	}
}

IRPCRecHitReader::~IRPCRecHitReader() {
	std::cout << "done" << std::endl;
}

DEFINE_FWK_MODULE(IRPCRecHitReader);
	
