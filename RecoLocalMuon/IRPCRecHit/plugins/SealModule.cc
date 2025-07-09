#include "FWCore/PluginManager/interface/ModuleDef.h"

#include "FWCore/Framework/interface/MakerMacros.h"


#include "RecoLocalMuon/IRPCRecHit/interface/IRPCRecHitAlgoFactory.h"

#include "RecoLocalMuon/IRPCRecHit/plugins/IRPCRecHitProducer.h"
DEFINE_FWK_MODULE(IRPCRecHitProducer);

//#include "RPCPointProducer.h"
//DEFINE_FWK_MODULE(RPCPointProducer);

#include "RecoLocalMuon/IRPCRecHit/plugins/IRPCRecHitStandardAlgo.h"
DEFINE_EDM_PLUGIN(IRPCRecHitAlgoFactory, IRPCRecHitStandardAlgo, "IRPCRecHitStandardAlgo");
