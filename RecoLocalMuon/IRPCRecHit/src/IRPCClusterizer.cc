#include "RecoLocalMuon/IRPCRecHit/interface/IRPCClusterizer.h"

/* IRPC */
#include "RecoLocalMuon/IRPCRecHit/interface/IRPCCluster.h"
#include "RecoLocalMuon/IRPCRecHit/interface/IRPCInfo.h"
#include "RecoLocalMuon/IRPCRecHit/interface/IRPCHit.h"
#include "RecoLocalMuon/IRPCRecHit/interface/IRPCHitContainer.h"

#include "DataFormats/IRPCDigi/interface/IRPCDigi.h"
#include "DataFormats/IRPCDigi/interface/IRPCDigiCollection.h"
#include "DataFormats/IRPCDigi/interface/IRPCDigiTime.h"

/* std */
#include <algorithm>
#include <limits>
#include <map>
#include <thread>
#include <numeric>

#include <iostream> // tests
#include <iomanip> // tests

IRPCClusterizer::IRPCClusterizer() {}
IRPCClusterizer::~IRPCClusterizer() {}

// *** commented out by juhee
IRPCClusterContainer IRPCClusterizer::doAction(const RPCRoll& roll, const IRPCDigiCollection::Range& digiRange, IRPCInfo& info)
{

// ************************** //
// *** save digis in hits *** //
// ************************** //

    IRPCClusterContainer clusters;

    // Return empty container for null input
	// *** digiRange std::pair<const_iterator, const_iterator>
	// *** distance: the number of elements between two iterators
    if(std::distance(digiRange.second, digiRange.first) == 0) return clusters;

    // Test output data
    //std::cout << "------------" << std::endl;
    //for(auto digi = digiRange.first; digi != digiRange.second; ++digi) {
    //  std::cout <<"strip=" << digi->strip() << " time=" <<  digi->time() << " position=" << digi->coordinateY() << " bx=" << digi->bx() << std::endl;
    //}

    std::map<int, std::pair<IRPCHitContainer, IRPCHitContainer>> hits; // <map<bunchX, std::pair<hr, lr>>
    auto it = hits.begin();

    //std::cout << "Clusterizer: " << std::endl;
    // Fill digi (simulation)
	//IRPCDigiTime time;
    for(auto digi = digiRange.first; digi != digiRange.second; ++digi) {
        int bunchX = digi->bx();
		int strip = digi->strip();
		float timeHR = abs(IRPCDigiTime( *digi ).timeHR());
		float timeLR = abs(IRPCDigiTime( *digi ).timeLR());

        it = hits.find(bunchX); 
		if(it == hits.end()) hits.insert(std::make_pair(bunchX, std::make_pair(IRPCHitContainer(), IRPCHitContainer())));

        hits.find(bunchX)->second.first.push_back(IRPCHit(strip, timeHR, bunchX));
        hits.find(bunchX)->second.first.back().setHR(true);

        hits.find(bunchX)->second.second.push_back(IRPCHit(strip, timeLR, bunchX));
        hits.find(bunchX)->second.second.back().setLR(true);

            //std::cout <<"strip=" << digi->strip() << " time=" <<  digi->time() << " position=" << digi->coordinateY() << " bx=" << digi->bx()  << " dt=" << timeHR - timeLR << std::endl;
    }
	IRPCClusterContainer cHR;
	IRPCClusterContainer cLR;
	IRPCClusterContainer cPair;

	for(auto & it: hits){
//		std::thread tHR(&IRPCClusterizer::oneSideCluster, this, 0.3, std::ref(it.second.first), std::ref(cHR));
//		std::thread tLR(&IRPCClusterizer::oneSideCluster, this, 0.3, std::ref(it.second.second), std::ref(cLR));
		std::thread tHR(&IRPCClusterizer::oneSideCluster, this, info.thrTimeHR(), std::ref(it.second.first), std::ref(cHR));
		std::thread tLR(&IRPCClusterizer::oneSideCluster, this, info.thrTimeLR(), std::ref(it.second.second), std::ref(cLR));
		tHR.join(); tLR.join();
	
		std::cout << "cHR size: " << cHR.size() << " cLR size: " << cLR.size() << std::endl;

		if(cHR.empty() && !cLR.empty()) return cLR;
		if(!cHR.empty() && cLR.empty()) return cHR;
//		// Compute clusters parameters HR
//		for(auto cl=cHR.begin(); cl!=cHR.end(); ++cl){
//			cl->compute(std::ref(info));
//		}
//		if (info.isOnlyHR()) return cHR;
//		// Compute clusters parameters LR
//		for(auto cl=cLR.begin(); cl!=cLR.end(); ++cl){
//			cl->compute(std::ref(info));
//		if (info.isOnlyLR()) return cLR;

		//if(cHR.size()>0 && cLR.size()>0){
		if(!cHR.empty() && !cLR.empty()){
			std::cout << std::endl << " == Let's do final cluster == " << std:: endl;
			cPair = finalCluster(cHR, cLR);
		}
	
		std::cout << "cHR.size: " << cHR.size() << " cLR.size: " << cLR.size() << std::endl;
		clusters.insert(clusters.end(), cPair.begin(), cPair.end());
		cHR.clear(); cLR.clear(); cPair.clear();
	}
    // Compute clusters parameters.
//    for(auto cl = clusters.begin(); cl != clusters.end(); ++cl)
//        cl->compute(std::ref(info));
//
	hits.clear();
	return clusters;
}

// *** IRPCHitContainer def: std::vector<IRPCHit>
// *** hitsOneSide: only HR or LR hits ex.hits.find(bunchX)->second.first;
bool IRPCClusterizer::oneSideCluster(float limit, IRPCHitContainer &hitsOneSide, IRPCClusterContainer &clusters ){
	
	if(hitsOneSide.size() == 0) return false;

	std::vector<int> indice(hitsOneSide.size());
	std::iota(indice.begin(), indice.end(), 0);
	
	// find the earlies time and its idx
	while (!indice.empty()){
		float minTime = std::numeric_limits<float>::max();
		int minTimeIdx = -1;
		for (int j=0; j < (int)indice.size(); ++j){
			float time = hitsOneSide.at(indice.at(j)).time();
			if (time < minTime){
				minTime = time;
				minTimeIdx = j;
			}
		}
		int minTimeStrip = hitsOneSide.at(indice.at(minTimeIdx)).strip();

		IRPCCluster tempHits;
		IRPCHit tempHit(minTimeStrip, minTime);
		tempHits.addHit(tempHit);

		std::vector<int> eraseIdx;
		eraseIdx.push_back(minTimeIdx);

		int maxStripJump = 1;
		int refIdx = -1;

		std::cout << std::endl << "minTimeIdx: " << minTimeIdx << " minTime: " << minTime << " minTimeStrip: " << minTimeStrip << std::endl;
		// Right side
		for (int j=minTimeIdx; j<(int)indice.size(); ++j){
	
			if (j==minTimeIdx){
				refIdx=minTimeIdx; 
				continue;
			}
			int refStrip = hitsOneSide.at(indice.at(refIdx)).strip();

			float time = hitsOneSide.at(indice.at(j)).time();
			int strip = hitsOneSide.at(indice.at(j)).strip();
			
			std::cout << "time: " << time << " strip: " << strip;
			if (std::abs(minTime-time) < limit){
				if (std::abs(refStrip - strip)<=1+maxStripJump){
					IRPCHit clusterHit(strip, time);
					tempHits.addHit(clusterHit);
					eraseIdx.push_back(j);
					refIdx = j;
					std::cout << " <== matched, refIdx has been updated to " << refIdx << std::endl;
				} else break;
			}
		}

		// Left side
		for (int j=minTimeIdx; j>=0; --j){
	
			if (j==minTimeIdx){
				refIdx=minTimeIdx; 
				continue;
			}
			int refStrip = hitsOneSide.at(indice.at(refIdx)).strip();

			float time = hitsOneSide.at(indice.at(j)).time();
			int strip = hitsOneSide.at(indice.at(j)).strip();
			
			std::cout << "time: " << time << " strip: " << strip;
			if (std::abs(minTime-time) < limit){
				if (std::abs(refStrip - strip)<=1+maxStripJump){
					IRPCHit clusterHit(strip, time);
					tempHits.addHit(clusterHit);
					eraseIdx.push_back(j);
					refIdx = j;
					std::cout << " <== matched, refIdx has been updated to " << refIdx << std::endl;
				} else break;
			}
		}

		std::sort(eraseIdx.begin(), eraseIdx.end(), std::greater<int>());
		for (int j: eraseIdx){
			indice.erase(indice.begin()+j);
		}
		clusters.push_back(tempHits);
	}
	return true;
}

IRPCClusterContainer IRPCClusterizer::finalCluster(IRPCClusterContainer HR, IRPCClusterContainer LR){

	IRPCClusterContainer clusters;

	int noMatch = 0;
	while (!LR.empty() && !HR.empty() && noMatch < (int)LR.size()+(int)HR.size()){
		float stripDiff = std::numeric_limits<float>::max();
		int idxHR = -1;
		int idxLR = -1;

		// Find the pair of clusters with the smallest strip differnce
		for (int ij=0; ij<(int)HR.size(); ++ij){
			for (int ji=0; ji<(int)LR.size(); ++ji){
				float diff = std::abs(HR.at(ij).deltaStrip() - LR.at(ji).deltaStrip());
				//std::cout << std::endl <<"HR dS: " << HR.at(ij).deltaStrip() << " LR dS: " << LR.at(ji).deltaStrip() << " diff: " <<diff ;
				if (diff < stripDiff){
					//std::cout << " << min diff " << std::endl;
					stripDiff = diff;
					idxHR = ij;
					idxLR = ji;
				}
			}
		}

		// If the smallest strip differnce is within the threshold, make a final cluster
		if (stripDiff < 0.9){
			//std::cout << "HR " << idxHR << " LR " << idxLR << " are matched! " << std::endl;
			IRPCCluster temp;
			temp.initialize(HR.at(idxHR), LR.at(idxLR));
			clusters.push_back(temp);

			HR.erase(HR.begin()+idxHR);
			LR.erase(LR.begin()+idxLR);
		} else {noMatch++;}
	}
	return clusters;
}

					
