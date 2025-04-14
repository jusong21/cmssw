import FWCore.ParameterSet.Config as cms

process = cms.Process("IRPCRecHitReader")

process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.load("Configuration.StandardSequences.GeometryRecoDB_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")

from Configuration.AlCa.GlobalTag import GlobalTag
#process.GlobalTag = GlobalTag(process.GlobalTag, "131X_mcRun4_realistic_v6", '')
process.GlobalTag = GlobalTag(process.GlobalTag, "auto:phase2_realistic_T21", "")

process.load("RecoLocalMuon.IRPCRecHit.irpcRecHits_cfi")

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
        "file:/eos/home-j/jusong/work/iRPC/CMSSW_13_3_0_pre2/src/RecoLocalMuon/IRPCRecHit/rechit01.root"
    )
)
process.TFileService = cms.Service("TFileService",
    fileName = cms.string("output.root"),
    closeFileFast = cms.untracked.bool(True)
)


process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(-1))

#process.irpcRecHitsLabel = cms.InputTag("irpcRecHits")

process.irpcRecHitReader = cms.EDAnalyzer("IRPCRecHitReader",
    #HistOutFile = cms.untracked.string("irpcRecHitHist.root"),
    irpcRecHitTag = cms.InputTag("irpcRecHits"),
#    recHitLabel1 = cms.untracked.string("irpcRecHits"), 
#    #recHitLabel1 = process.irpcRecHitsLabel,
#    recHitLabel2 = cms.untracked.string(""), 
#
#    trigIRPC1 = cms.untracked.bool(True),
#    trigIRPC2 = cms.untracked.bool(True),
#    trigIRPC3 = cms.untracked.bool(True),
#    trigIRPC4 = cms.untracked.bool(True),
#    trigIRPC5 = cms.untracked.bool(True),
#    trigIRPC6 = cms.untracked.bool(True),
)

process.p = cms.Path(process.irpcRecHitReader)

#process.schedule = cms.Schedule(process.p)

