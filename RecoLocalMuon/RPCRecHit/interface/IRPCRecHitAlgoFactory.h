#ifndef RecoLocalMuon_IRPCRecHitAlgoFactory_H
#define RecoLocalMuon_IRPCRecHitAlgoFactory_H

/** \class IRPCRecHitAlgoFactory
 *  Factory of seal plugins for 1D RecHit reconstruction algorithms.
 *  The plugins are concrete implementations of RPCRecHitBaseAlgo base class.
 *
 *  \author original version: G. Cerminara - INFN Torino
 *  \adapted by Juhee Song (Hanyang Univ, VUB)
 */
#include "FWCore/PluginManager/interface/PluginFactory.h"
#include "RecoLocalMuon/RPCRecHit/interface/IRPCRecHitBaseAlgo.h"

typedef edmplugin::PluginFactory<IRPCRecHitBaseAlgo *(const edm::ParameterSet &)> IRPCRecHitAlgoFactory;
#endif
