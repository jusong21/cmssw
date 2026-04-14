import FWCore.ParameterSet.Config as cms

rpcRecHitsPhase2 = cms.EDProducer(
    "RPCRecHitPhase2Producer",
    rpcDigiPhase2Label=cms.InputTag("simMuonRPCDigisPhase2"),
    irpcDigiLabel=cms.InputTag("simMuonIRPCDigis"),
    useIRPC=cms.bool(True),
    irpcThrTime=cms.double(1.0e-5),
    irpcThrStripNum=cms.double(0.9),
    irpcSpeed=cms.double(19.786302),
)
