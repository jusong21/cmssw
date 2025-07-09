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

	//info.setThrTimeHR(3);
	//info.setThrTimeLR(3);
	//float thrStripNum = 0.9;

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

			std::cout << " - n" << nCluHR << "  bx: " << cl.bx() << " fst: " << cl.firstStrip() << " lst: " << cl.lastStrip() << " cluSize: " << cl.clusterSize() << " nSt: " << cl.nStrip() << " StNumAvg: " << cl.stripNumAvg() << std::endl;
			std::cout << "       highT: " << cl.highTime() << " highTErr: " << cl.highTimeRMS() << " lowT: " << cl.lowTime() << " lowTErr: " << cl.lowTimeRMS() << std::endl;
			std::cout << "       y: " << cl.y() << " yErr: " << cl.yRMS() << " x: " << cl.x() << " xD: " << cl.xD() << std::endl;
			std::cout << std::endl;
		}
		if (info.isOnlyHR()) return clustersHR;
		
		std::cout << std::endl;
		std::cout << " OneSide LR clusters..." << std::endl;
		int nCluLR = 0;
		for (auto cl: clustersLR){
			cl.compute( std::ref(info) );
			nCluLR++;

			std::cout << " - n" << nCluLR << "  bx: " << cl.bx() << " fst: " << cl.firstStrip() << " lst: " << cl.lastStrip() << " cluSize: " << cl.clusterSize() << " nSt: " << cl.nStrip() << " StNumAvg: " << cl.stripNumAvg() << std::endl;
			std::cout << "       highT: " << cl.highTime() << " highTErr: " << cl.highTimeRMS() << " lowT: " << cl.lowTime() << " lowTErr: " << cl.lowTimeRMS() << std::endl;
			std::cout << "       y: " << cl.y() << " yErr: " << cl.yRMS() << " x: " << cl.x() << " xD: " << cl.xD() << std::endl;
			std::cout << std::endl;
		}
		if (info.isOnlyLR()) return clustersLR;
		
		// final clustering
		std::cout << " Final clustering..." << std::endl;
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
			} else { nomatch = true; std::cout << "nomatch Break!" << std::endl; break; }
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
			} else { nomatch = true; std::cout << "nomatch Break!" << std::endl; break; }
		}

		clusters.push_back(tempCluster);
		tempCluster.hits()->clear();
	}
	return true;
}


//	auto &hitCont = oneSideHitCont;
//	IRPCCluster tempCluster;
//
//	int nhits = 0;
//
//	// flags 
//	std::vector<std::pair<IRPCHit*, int>> hitsFlags;
//	hitsFlags.reserve(hitCont.size()); // memory
//	for (auto &hit: hitCont){
//		std::lock_guard<std::mutex> lock(clusterMutex);
//		hitsFlags.emplace_back(&hit, 0);
//	}
//
//	while (std::any_of(hitsFlags.begin(), hitsFlags.end(), 
//	                  [](const auto& hitFlag) { return hitFlag.second == 0; })) {
//		// Find the earliest signal
//	    float minTime = INFINITY;
//	    size_t minTimeIdx = hitsFlags.size(); 
//	    for (size_t i = 0; i < hitsFlags.size(); ++i) {
//	        if (hitsFlags[i].second == 0 && hitsFlags[i].first->time() < minTime) {
//	            minTime = hitsFlags[i].first->time();
//	            minTimeIdx = i;
//        	}
//    	}
//
//	    if (minTimeIdx == hitsFlags.size()) break; // No more hits
//	    nhits++;
//
//		auto minTimeHit = hitsFlags[minTimeIdx].first;
//    	//tempCluster.addHit(*minTimeHit);
//		//tempCluster.addHit(const_cast<IRPCHit&>(*hitsFlags[minTimeIdx].first));
//		tempCluster.addHit(const_cast<IRPCHit&>(*minTimeHit));
//    	hitsFlags[minTimeIdx].second = -1; 
//	
//		int maxStripJump = 1;
//	    size_t currentIdx = minTimeIdx;
//		int matched = 1; bool nomatch = false;
//	    while (!nomatch) {
//	        size_t leftIdx = currentIdx - 1;
//			float prevTime = hitsFlags[currentIdx].first->time();
//			float nowTime = hitsFlags[leftIdx].first->time();
//
//	        IRPCHit* leftHit = hitsFlags[leftIdx].first;
//			if(leftHit->isAdjacentStrip( *hitsFlags[currentIdx].first, maxStripJump)
//			   && leftHit->isAdjacentTime( *hitsFlags[minTimeIdx].first, thrTime)
//			   && prevTime <= nowTime){
//				tempCluster.addHit( *leftHit );
//				hitsFlags[leftIdx].second = -1;
//				--currentIdx;
//				++matched;
//			} else { nomatch = true; std::cout << "nomatch Break!" << std::endl; break; }
//		}
//
//	    currentIdx = minTimeIdx;
//		nomatch = false;
//	    while (!nomatch) {
//	        size_t rightIdx = currentIdx + 1;
//			float prevTime = hitsFlags[currentIdx].first->time();
//			float nowTime = hitsFlags[rightIdx].first->time();
//
//	        IRPCHit* rightHit = hitsFlags[rightIdx].first;
//			if(rightHit->isAdjacentStrip( *hitsFlags[currentIdx].first, maxStripJump)
//			   && rightHit->isAdjacentTime( *hitsFlags[minTimeIdx].first, thrTime)
//			   && prevTime <= nowTime){
//				tempCluster.addHit( *rightHit );
//				hitsFlags[rightIdx].second = -1;
//				++currentIdx;
//				matched++;
//			} else { nomatch = true; std::cout << "nomatch Break!" << std::endl; break; }
//		}
//
//		clusters.push_back(tempCluster);
//		tempCluster.hits()->clear();
//	}
//	return true;
//}

//
//
//
//        while (leftHit != hitCont.begin()) {
//			float prevTime = leftHit->time();
//            --leftHit;
//			float nowTime = leftHit->time();
//			std::cout << "ref hit strip: " << stripRefLeftHit->strip() << "   left hit strip: " << leftHit->strip() << std::endl;
//			std::cout << "min hit time: " << minTimeHit->time() << "   left hit time: " << leftHit->time() << std::endl;
//			std::cout << "prevTime: " << prevTime << " nowTime: " << nowTime << std::endl;
//			if ( leftHit->isAdjacentStrip( *stripRefLeftHit, maxStripJump ) ) {
//				if ( leftHit->isAdjacentTime( *minTimeHit, thrTime ) && (nowTime >= prevTime) ) {
//					tempCluster.addHit( *leftHit );
//					--stripRefLeftHit;
//					std::cout << " <=== Matched! === \n" << std::endl; 
//					matched++;
//				}
//			} else {std::cout << "Break!\n" << std::endl; } break;
//		}
//		
//
//
//
//
//
//
//
//	while (!hitCont.empty()){
//		nhits++;
//		//std::cout << "nloops: " << nhits << std::endl;
//		float minTime = std::numeric_limits<float>::max();
//		auto minTimeHit = hitCont.end();
//	
//		//std::cout << "finding the earliest time and strip... " << std::endl;
//		// find the earliest time and its strip, idx
//		for (auto hit=hitCont.begin(); hit!=hitCont.end(); ++hit){
//			//std::cout << "temp T: " << hit->time()  << "   min T: " << minTime << std::endl;
//			if ( hit->time() < minTime ){
//				minTime = hit->time();
//				minTimeHit = hit;
//				//std::cout << "min T is updated to " << hit->time() << std::endl;
//			}
//		}
//		//std::cout << "* min time: " << minTime << std::endl;
//
//		tempCluster.addHit(*minTimeHit);
//
//		auto leftHit = minTimeHit, stripRefLeftHit = minTimeHit;
//		auto rightHit = minTimeHit, stripRefRightHit = minTimeHit;
//		int maxStripJump = 1;
//
//		std::cout << std::endl;
//		std::cout << "  !  new loop  !" << std::endl;
//		std::cout << "checking leftside..." << std::endl;
//		int matched = 1;
//		// check left
//        while (leftHit != hitCont.begin()) {
//			float prevTime = leftHit->time();
//            --leftHit;
//			float nowTime = leftHit->time();
//			std::cout << "ref hit strip: " << stripRefLeftHit->strip() << "   left hit strip: " << leftHit->strip() << std::endl;
//			std::cout << "min hit time: " << minTimeHit->time() << "   left hit time: " << leftHit->time() << std::endl;
//			std::cout << "prevTime: " << prevTime << " nowTime: " << nowTime << std::endl;
//			if ( leftHit->isAdjacentStrip( *stripRefLeftHit, maxStripJump ) ) {
//				if ( leftHit->isAdjacentTime( *minTimeHit, thrTime ) && (nowTime >= prevTime) ) {
//					tempCluster.addHit( *leftHit );
//					--stripRefLeftHit;
//					std::cout << " <=== Matched! === \n" << std::endl; 
//					matched++;
//				}
//			} else {std::cout << "Break!\n" << std::endl; } break;
//		}
//		
////		std::cout << std::endl;
//		std::cout << "checking rightside..." << std::endl;
//		// check right
//        while (rightHit != hitCont.end()-1) {
//			float prevTime = rightHit->time();
//            ++rightHit; 
//			float nowTime = rightHit->time();
//			std::cout << "ref hit strip: " << stripRefRightHit->strip() << "   right hit strip: " << rightHit->strip() << std::endl;
//			std::cout << "min hit time: " << minTimeHit->time() << "   right hit time: " << rightHit->time() << std::endl;
//			std::cout << "prevTime: " << prevTime << " nowTime: " << nowTime << std::endl;
//			if ( rightHit->isAdjacentStrip( *stripRefRightHit, maxStripJump ) ) {
//				if ( rightHit->isAdjacentTime( *minTimeHit, thrTime ) && (nowTime >= prevTime) ) {
//					tempCluster.addHit( *rightHit );
//					++stripRefRightHit;
//					std::cout << " <=== Matched! === \n" << std::endl; 
//					matched++;
//				}
//			//} else break;
//			} else {std::cout << "Break!\n" << std::endl; } break;
//		}
//
////		std::cout << std::endl;
////		std::cout << "checking cluster... nMatched: " << matched << std::endl;
////		for (auto h: *tempCluster.hits()){
////			std::cout << "bx " << h.bx() << " time " << h.time() << " st " << h.strip() << std::endl;
////		}
////		std::cout << std::endl;
//
//
//		clusters.push_back(tempCluster);
//		for (auto eraseHit: *tempCluster.hits()) {
//			hitCont.erase(std::remove(hitCont.begin(), hitCont.end(), eraseHit), hitCont.end());
//		}
//		tempCluster.hits()->clear();
//	}


IRPCClusterContainer IRPCClusterizer::finalClusterizer(IRPCClusterContainer HR, IRPCClusterContainer LR, float thrStripNum){

	IRPCClusterContainer clusters;
	
	IRPCCluster tempCluster;

	int noMatch = 0;
	int matched = 0;
	
	// checking
	std::cout << " - nHRc: " << HR.size() << " nLRc: " << LR.size() << std::endl;

	for (auto clHR=HR.begin(); clHR!=HR.end(); ++clHR) {
		for (auto clLR=LR.begin(); clLR!=LR.end(); ++clLR) {
			float stripHR = clHR->stripNumAvg();
			float stripLR = clLR->stripNumAvg();
			float deltaStrip = std::abs( stripHR-stripLR );

			std::cout << "  - stripHR: " << stripHR << " stripLR: " << stripLR << " dS: " << deltaStrip << std::endl;

			if (deltaStrip < thrStripNum) {
				tempCluster.initialize(*clHR, *clLR);
				clusters.push_back(tempCluster);
				matched++;
			}
		}
	}

//				
//	while( !HR.empty() && !LR.empty() && noMatch==0 ){
//
//		float minDeltaStrip = std::numeric_limits<float>::max();
//		IRPCCluster minHR, minLR;
//	
//		for (auto clHR=HR.begin(); clHR!=HR.end(); ++clHR){
//			//int matched = 0;
//			for (auto clLR=LR.begin(); clLR!=LR.end(); ++clLR){
//				float stripHR = clHR->stripNumAvg();
//				float stripLR = clLR->stripNumAvg();
//				float deltaStrip = std::abs( stripHR-stripLR );
//
//				std::cout << "  - stripHR: " << stripHR << " stripLR: " << stripLR << " dS: " << deltaStrip << std::endl;
//				
//				if (deltaStrip < thrStripNum) {
//					minHR = *clHR;
//					minLR = *clLR;
//
//				if (deltaStrip > thrStripNum) continue;
//
//				if (deltaStrip < minDeltaStrip){
//					minDeltaStrip = deltaStrip;
//					minHR = *clHR;
//					minLR = *clLR;
//					//matched++;
//				}
//				// std::cout << "min dS: " << minDeltaStrip << std::endl;
//			}
//			// std::cout << "mat " << matched << std::endl;
//		}
//
//		std::cout << std::endl << " - HR dS: " << minHR.stripNumAvg() << " LR dS: " << minLR.stripNumAvg() << " dS: " << minDeltaStrip;
//		if (minDeltaStrip < thrStripNum) std::cout << " < ====  Matched ";
//		std::cout << std::endl;
//
//		if (minDeltaStrip < thrStripNum){
//			tempCluster.initialize(minHR, minLR);
//			clusters.push_back(tempCluster);
//			//HR.erase( std::remove(HR.begin(), HR.end(), minHR), HR.end() );
//			//LR.erase( std::remove(LR.begin(), LR.end(), minLR), LR.end() );
//			HR.erase(std::find(HR.begin(), HR.end(), minHR));
//			LR.erase(std::find(LR.begin(), LR.end(), minLR));
//			tempCluster.hits()->clear();
//			matched++;
//		} else {noMatch++;} 
	std::cout << " - noMat: " << noMatch << " mat: " << matched << " HR empty? " << HR.empty() << " LR empty? " << LR.empty() << std::endl;
	std::cout << std::endl << std::endl;

	//}
	return clusters;
}
