/** \Class IRPCMaskReClusterizer
 *  \author Juhee Song
 */

#include "RecoLocalMuon/IRPCRecHit/interface/IRPCCluster.h"
#include "RecoLocalMuon/IRPCRecHit/interface/IRPCClusterizer.h"
#include "RecoLocalMuon/IRPCRecHit/interface/IRPCMaskReClusterizer.h"

IRPCClusterContainer IRPCMaskReClusterizer::doAction(const RPCDetId& id,
                                                   IRPCClusterContainer& initClusters,
                                                   const RollMask& mask) const {
  IRPCClusterContainer finClusters;
  if (initClusters.empty())
    return finClusters;

  IRPCCluster prev = *initClusters.begin();
  for (auto cl = std::next(initClusters.begin()); cl != initClusters.end(); ++cl) {
	//// below need to be checked for IRPC
    // Merge this cluster if it is adjacent by 1 masked strip
    // Note that the IRPCClusterContainer collection is sorted in DECREASING ORDER of strip #
    // So the prev. cluster is placed after the current cluster (check the < operator of IRPCCluster carefully)
    if ((prev.firstStrip() - cl->lastStrip()) == 2 and this->get(mask, cl->lastStrip() + 1) and prev.bx() == cl->bx()) {
      IRPCCluster merged(cl->firstStrip(), prev.lastStrip(), cl->bx());
      prev = merged;
    } else {
      finClusters.insert(prev);
      prev = *cl;
    }
  }

  // Finalize by putting the last cluster to the collection
  finClusters.insert(prev);

  return finClusters;
}

bool IRPCMaskReClusterizer::get(const RollMask& mask, int strip) const { return mask.test(strip - 1); }
