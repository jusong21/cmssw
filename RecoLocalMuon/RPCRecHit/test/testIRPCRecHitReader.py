import FWCore.ParameterSet.Config as cms

process = cms.Process("IRPCRecHitReader")

process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.load("Configuration.StandardSequences.GeometryRecoDB_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")

from Configuration.AlCa.GlobalTag import GlobalTag
#process.GlobalTag = GlobalTag(process.GlobalTag, "131X_mcRun4_realistic_v6", '')
process.GlobalTag = GlobalTag(process.GlobalTag, "auto:phase2_realistic_T21", "")

process.load("RecoLocalMuon.RPCRecHit.irpcRecHits_cfi")

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
        "file:/eos/home-j/jusong/work/iRPC/test/CMSSW_15_1_0_pre3/src/step2.root"
    )
)
process.TFileService = cms.Service("TFileService",
    fileName = cms.string("rechit_hist.root"),
    closeFileFast = cms.untracked.bool(True)
)


process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(-1))

#process.irpcRecHitsLabel = cms.InputTag("irpcRecHits")

process.irpcRecHitReader = cms.EDAnalyzer("IRPCRecHitReader",
    #HistOutFile = cms.untracked.string("irpcRecHitHist.root"),
    irpcRecHitTag = cms.InputTag("irpcRecHits"),
    validateClustering = cms.bool(True),
    irpcDigiTag = cms.InputTag("simMuonIRPCDigis"),
    thrTime = cms.double(0.00001),
    thrStripNum = cms.double(0.9),
)

process.p = cms.Path(process.irpcRecHits * process.irpcRecHitReader)

#process.schedule = cms.Schedule(process.p)

