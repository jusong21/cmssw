
import FWCore.ParameterSet.Config as cms

irpcRecHits = cms.EDProducer("IRPCRecHitProducer",
    recAlgoConfig = cms.PSet(

    ),
    recAlgo = cms.string('IRPCRecHitStandardAlgo'),
    irpcDigiTag = cms.InputTag('simMuonIRPCDigis'),
    thrTime = cms.double(3),
    thrStripNum = cms.double(0.9)
)
#FIXME I don't understand below
##disabling DIGI2RAW,RAW2DIGI chain for Phase2
#from Configuration.Eras.Modifier_phase2_muon_cff import phase2_muon
#phase2_muon.toModify(rpcRecHits, rpcDigiLabel = 'simMuonRPCDigis')
