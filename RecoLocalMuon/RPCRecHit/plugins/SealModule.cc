#include "FWCore/PluginManager/interface/ModuleDef.h"

#include "FWCore/Framework/interface/MakerMacros.h"


#include "RPCRecHitProducer.h"
#include "RPCPointProducer.h"
#include "RecoLocalMuon/RPCRecHit/plugins/IRPCRecHitProducer.h"

#include "RPCRecHitAlgoFactory.h"
#include "RPCRecHitStandardAlgo.h"
#include "RecoLocalMuon/RPCRecHit/interface/IRPCRecHitAlgoFactory.h"
#include "RecoLocalMuon/RPCRecHit/interface/IRPCRecHitStandardAlgo.h"

DEFINE_FWK_MODULE(RPCRecHitProducer);
DEFINE_FWK_MODULE(RPCPointProducer);
DEFINE_FWK_MODULE(IRPCRecHitProducer);
DEFINE_EDM_PLUGIN(RPCRecHitAlgoFactory, RPCRecHitStandardAlgo, "RPCRecHitStandardAlgo");
DEFINE_EDM_PLUGIN(IRPCRecHitAlgoFactory, IRPCRecHitStandardAlgo, "IRPCRecHitStandardAlgo");
