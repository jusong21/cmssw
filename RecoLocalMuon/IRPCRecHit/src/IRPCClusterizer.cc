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

	// test hits
	std::cout << " Checking stored hits... " << std:: endl;
	for (auto & [bx, hitCont] : hits){

		IRPCHitContainer hr = hitCont.first;
		IRPCHitContainer lr = hitCont.second;

		std::cout << std::endl << " - bx: " << bx;
		// each hit cont
		std::cout << std::endl << "  Check HR hits..." << std::endl;
		for (auto & h: hr){
			std::cout << "  - bx: " << h.bx() << " strip: " << h.strip() << " time: " << h.time() << std::endl;
		}

		std::cout << std::endl << "  Check LR hits..." << std::endl;
		for (auto & h: lr){
			std::cout << "  - bx: " << h.bx() << " strip: " << h.strip() << " time: " << h.time() << std::endl;
		}
	}
	std::cout << std::endl;

	IRPCClusterContainer clustersHR, clustersLR;
	IRPCClusterContainer finalClusters;

	for (auto & [bx, hitCont]: hits){

		// one-side clustering
		std::thread threadHR( &IRPCClusterizer::oneSideClusterizer, this, info.thrTimeHR(), std::ref(hitCont.first),  std::ref(clustersHR) );
		std::thread threadLR( &IRPCClusterizer::oneSideClusterizer, this, info.thrTimeLR(), std::ref(hitCont.second), std::ref(clustersLR) );
		threadHR.join(); threadLR.join();

		std::cout << std::endl;
		std::cout << " OneSide HR clusters..." << std::endl;
		int nCluHR = 0;
		for (auto cl: clustersHR){
			cl.compute( std::ref(info) );
			nCluHR++;

//			std::cout << " - n" << nCluHR << "  bx: " << cl.bx() << " fst: " << cl.firstStrip() << " lst: " << cl.lastStrip() << " cluSize: " << cl.clusterSize() << " nSt: " << cl.nStrip() << " StNumAvg: " << cl.stripNumAvg() << std::endl;
//			std::cout << "       highT: " << cl.highTime() << " highTErr: " << cl.highTimeRMS() << " lowT: " << cl.lowTime() << " lowTErr: " << cl.lowTimeRMS() << std::endl;
//			std::cout << "       y: " << cl.y() << " yErr: " << cl.yRMS() << " x: " << cl.x() << " xD: " << cl.xD() << std::endl;
//			std::cout << std::endl;
		}
		if (info.isOnlyHR()) return clustersHR;
		
//		std::cout << std::endl;
//		std::cout << " OneSide LR clusters..." << std::endl;
		int nCluLR = 0;
		for (auto cl: clustersLR){
			cl.compute( std::ref(info) );
			nCluLR++;

//			std::cout << " - n" << nCluLR << "  bx: " << cl.bx() << " fst: " << cl.firstStrip() << " lst: " << cl.lastStrip() << " cluSize: " << cl.clusterSize() << " nSt: " << cl.nStrip() << " StNumAvg: " << cl.stripNumAvg() << std::endl;
//			std::cout << "       highT: " << cl.highTime() << " highTErr: " << cl.highTimeRMS() << " lowT: " << cl.lowTime() << " lowTErr: " << cl.lowTimeRMS() << std::endl;
//			std::cout << "       y: " << cl.y() << " yErr: " << cl.yRMS() << " x: " << cl.x() << " xD: " << cl.xD() << std::endl;
//			std::cout << std::endl;
		}
		if (info.isOnlyLR()) return clustersLR;
		
		// final clustering
//		std::cout << " Final clustering..." << std::endl;
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
	
	IRPCCluster tempCluster;

	int noMatch = 0;
	int matched = 0;
	
	// checking
//	std::cout << " - nHRc: " << HR.size() << " nLRc: " << LR.size() << std::endl;

	for (auto clHR=HR.begin(); clHR!=HR.end(); ++clHR) {
		for (auto clLR=LR.begin(); clLR!=LR.end(); ++clLR) {
			float stripHR = clHR->stripNumAvg();
			float stripLR = clLR->stripNumAvg();
			float deltaStrip = std::abs( stripHR-stripLR );

//			std::cout << "  - stripHR: " << stripHR << " stripLR: " << stripLR << " dS: " << deltaStrip << std::endl;

			if (deltaStrip < thrStripNum) {
				tempCluster.initialize(*clHR, *clLR);
				clusters.push_back(tempCluster);
				matched++;
			}
		}
	}

//	std::cout << " - noMat: " << noMatch << " mat: " << matched << " HR empty? " << HR.empty() << " LR empty? " << LR.empty() << std::endl;
//	std::cout << std::endl << std::endl;

	return clusters;
}
