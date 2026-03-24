/*
 * File: IRPCClusterizer.cc
 *
 * Author: Juhee Song (Hanyang Univ, Vrije Universiteit Brussel)
 * Creation: 
 *
 * Original version author: Roumyana Mileva Hadjiiska
 * This alsgorithm is inspired on Shchablo Konstantin
 *
 */
#include "RecoLocalMuon/IRPCRecHit/interface/IRPCClusterizer.h"
#include "RecoLocalMuon/IRPCRecHit/interface/IRPCCluster.h"
#include "RecoLocalMuon/IRPCRecHit/interface/IRPCInfo.h"
#include "RecoLocalMuon/IRPCRecHit/interface/IRPCHit.h"
#include "RecoLocalMuon/IRPCRecHit/interface/IRPCHitContainer.h"
#include "DataFormats/RPCDigi/interface/IRPCDigi.h"
#include "DataFormats/RPCDigi/interface/IRPCDigiCollection.h"
#include "DataFormats/RPCDigi/interface/IRPCDigiTime.h"

#include <algorithm>
#include <limits>
#include <map>
#include <thread>
#include <numeric>
#include <mutex>

#include <iostream> 
#include <iomanip> 

IRPCClusterizer::IRPCClusterizer() {}
IRPCClusterizer::~IRPCClusterizer() {}

IRPCClusterContainer IRPCClusterizer::doAction( const IRPCDigiCollection::Range& digiRange, IRPCInfo& info ){ // digiRange std::pair<const_iterator, const_iterator>

    IRPCClusterContainer clusters;

    if (std::distance(digiRange.second, digiRange.first) == 0) return clusters; // Return empty container for null input

    std::map<int, std::pair<IRPCHitContainer, IRPCHitContainer>> hits; // <map<bunchX, std::pair<hr, lr>>
    auto it = hits.begin();

	// IRPCDigiTime time;
	// save digis in hits 
    for (auto digi = digiRange.first; digi != digiRange.second; ++digi) {
        int bunchX = digi->bx();
		int strip = digi->strip();
		float timeHR = IRPCDigiTime( *digi ).timeHR();
		float timeLR = IRPCDigiTime( *digi ).timeLR();
		
//		std::cout << "  ========------ digi info ---------======" << std::endl;
//		std::cout << " - bx: " << digi->bx() << " sbx: " << digi->sbx() << std::endl;
//		std::cout << " - HR bx: " << digi->bxHR() << " sbx: " << digi->sbxHR() << " ft: " << digi->tHR() << " time: " << timeHR << std::endl;
//		std::cout << " - LR bx: " << digi->bxLR() << " sbx: " << digi->sbxLR() << " ft: " << digi->tLR() << " time: " << timeLR << std::endl;

        it = hits.find(bunchX); 
		if (it==hits.end()){
			hits.insert( std::make_pair(bunchX, std::make_pair( IRPCHitContainer(), IRPCHitContainer() )) );
		}
	    hits.find(bunchX)->second.first.push_back(IRPCHit(strip, timeHR, bunchX));
	    hits.find(bunchX)->second.first.back().setHR(true);
	    hits.find(bunchX)->second.second.push_back(IRPCHit(strip, timeLR, bunchX));
	    hits.find(bunchX)->second.second.back().setLR(true);
//		if (timeHR!=0){
//	        hits.find(bunchX)->second.first.push_back(IRPCHit(strip, timeHR, bunchX));
//	        hits.find(bunchX)->second.first.back().setHR(true);
//		}
//		if (timeLR!=0){
//	        hits.find(bunchX)->second.second.push_back(IRPCHit(strip, timeLR, bunchX));
//	        hits.find(bunchX)->second.second.back().setLR(true);
//		}
		// test digi output
        // std::cout <<"strip=" << digi->strip() << " time=" <<  digi->time() << " position=" << digi->coordinateY() << " bx=" << digi->bx()  << " dt=" << timeHR - timeLR << std::endl << std::endl;
    }

	IRPCClusterContainer clustersHR, clustersLR;
	IRPCClusterContainer finalClusters;

	for (auto & [bx, hitCont]: hits){

		// one-side clustering
		// NOTE:
		// The previous implementation spawned threads that push into shared
		// vectors (clustersHR/clustersLR). That is not thread-safe and can
		// lead to non-deterministic behavior. We do it sequentially here.
		(void)oneSideClusterizer(info.thrTimeHR(), hitCont.first, clustersHR);
		(void)oneSideClusterizer(info.thrTimeLR(), hitCont.second, clustersLR);
		if (info.isOnlyHR()) return clustersHR;
		
		if (info.isOnlyLR()) return clustersLR;
		
		// final clustering
		if (!clustersHR.empty() && !clustersLR.empty()){
			finalClusters = IRPCClusterizer::finalClusterizer(clustersHR, clustersLR, info.thrStripNum());
		}

		// std::cout << "cHR.size: " << cHR.size() << " cLR.size: " << cLR.size() << std::endl;
		clusters.insert(clusters.end(), finalClusters.begin(), finalClusters.end());
		clustersHR.clear(); clustersLR.clear(); finalClusters.clear();
	}
    // Compute clusters parameters.
    for (auto cl=clusters.begin(); cl!=clusters.end(); ++cl){
        cl->compute( std::ref(info) );
	}

	hits.clear();
	return clusters;
}

bool IRPCClusterizer::oneSideClusterizer(float thrTime, IRPCHitContainer &oneSideHitCont, IRPCClusterContainer &clusters ){

	if (oneSideHitCont.size()==0) return false;

	//IRPCHitContainer& hitContRef = hitCont;
	auto &hitCont = oneSideHitCont;
	IRPCCluster tempCluster;

	int nhits = 0;

	std::vector<std::pair<size_t, int>> hitsFlags; // <index, flag>
	hitsFlags.reserve(hitCont.size());

    for (size_t i = 0; i < hitCont.size(); ++i) {
        hitsFlags.emplace_back(i, 0); 
    }

	while (std::any_of(hitsFlags.begin(), hitsFlags.end(), 
	                  [](const auto& hitFlag) { return hitFlag.second == 0; })) {
		// Find the earliest signal
	    float minTime = INFINITY;
	    size_t minTimeIdx = hitsFlags.size(); 
	    for (size_t i = 0; i < hitsFlags.size(); ++i) {
			if (hitsFlags[i].second == 0){
				const auto& hit = hitCont[hitsFlags[i].first];
				if (hit.time() < minTime) {
					minTime = hit.time();
					minTimeIdx = i;
				}
			}
		}

		if (minTimeIdx == hitsFlags.size()) break;
		++nhits;
	
		auto minTimeHit = hitCont[hitsFlags[minTimeIdx].first];
		tempCluster.addHit(minTimeHit);
		hitsFlags[minTimeIdx].second = -1;

		int maxStripJump = 1;
	    size_t currentIdx = hitsFlags[minTimeIdx].first;
		int matched = 1; bool nomatch = false;
	    while (!nomatch) {
			if (currentIdx == 0) break; // prevent size_t underflow
	        size_t leftIdx = currentIdx - 1;
			IRPCHit leftHit = hitCont[leftIdx];
			IRPCHit refHit = hitCont[currentIdx];
			
			float prevTime = refHit.time();
			float nowTime = leftHit.time();

			if(leftHit.isAdjacentStrip( refHit, maxStripJump)
			   && leftHit.isAdjacentTime( hitCont[hitsFlags[minTimeIdx].first], thrTime)
			   && prevTime <= nowTime){
				tempCluster.addHit( leftHit );
				hitsFlags[leftIdx].second = -1;
				--currentIdx;
				++matched;
			} else { nomatch = true; /*std::cout << "nomatch Break!" << std::endl;*/ break; }
		}

	    currentIdx = hitsFlags[minTimeIdx].first;
		nomatch = false;
	    while (!nomatch) {
			if (currentIdx + 1 >= hitCont.size()) break; // prevent out-of-range
	        size_t rightIdx = currentIdx + 1;
			IRPCHit rightHit = hitCont[rightIdx];
			IRPCHit refHit = hitCont[currentIdx];
			
			float prevTime = refHit.time();
			float nowTime = rightHit.time();

			if(rightHit.isAdjacentStrip( refHit, maxStripJump)
			   && rightHit.isAdjacentTime( hitCont[hitsFlags[minTimeIdx].first], thrTime)
			   && prevTime <= nowTime){
				tempCluster.addHit( rightHit );
				hitsFlags[rightIdx].second = -1;
				++currentIdx;
				++matched;
			} else { nomatch = true;/* std::cout << "nomatch Break!" << std::endl;*/ break; }
		}

		clusters.push_back(tempCluster);
		tempCluster.hits()->clear();
	}
	return true;
}

IRPCClusterContainer IRPCClusterizer::finalClusterizer(IRPCClusterContainer HR, IRPCClusterContainer LR, float thrStripNum){

	IRPCClusterContainer clusters;

	// Greedy 1:1 matching:
	// - consider all (HR_i, LR_j) with deltaStrip < thrStripNum
	// - sort by deltaStrip ascending
	// - pick pairs in that order, each HR and LR cluster can be used at most once
	struct Candidate {
		size_t iHR;
		size_t iLR;
		float deltaStrip;
	};

	std::vector<Candidate> candidates;
	candidates.reserve(HR.size() * LR.size());

	for (size_t iHR = 0; iHR < HR.size(); ++iHR) {
		for (size_t iLR = 0; iLR < LR.size(); ++iLR) {
			const float stripHR = HR[iHR].stripNumAvg();
			const float stripLR = LR[iLR].stripNumAvg();
			const float deltaStrip = std::abs(stripHR - stripLR);
			if (deltaStrip < thrStripNum) {
				candidates.push_back(Candidate{iHR, iLR, deltaStrip});
			}
		}
	}

	std::sort(candidates.begin(), candidates.end(),
	          [](const Candidate& a, const Candidate& b) {
		          if (a.deltaStrip != b.deltaStrip) return a.deltaStrip < b.deltaStrip;
		          if (a.iHR != b.iHR) return a.iHR < b.iHR;
		          return a.iLR < b.iLR;
	          });

	std::vector<char> usedHR(HR.size(), 0);
	std::vector<char> usedLR(LR.size(), 0);

	for (const auto& cand : candidates) {
		if (usedHR[cand.iHR] || usedLR[cand.iLR]) continue;

		IRPCCluster tempCluster;
		tempCluster.initialize(HR[cand.iHR], LR[cand.iLR]);
		clusters.push_back(tempCluster);

		usedHR[cand.iHR] = 1;
		usedLR[cand.iLR] = 1;
	}

	return clusters;
}
