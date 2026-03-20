import FWCore.ParameterSet.Config as cms

process = cms.Process("demo")

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

process.source = cms.Source("PoolSource",
                                fileNames = cms.untracked.vstring('file:/eos/home-j/jusong/work/iRPC/CMSSW_15_1_0_pre3/src/IRPC/step2_5000.root')
                            )
process.TFileService = cms.Service("TFileService",
	fileName = cms.string("digi_output.root"),
	closeFileFast = cms.untracked.bool(True))

process.demo = cms.EDAnalyzer("IRPCDigiAnalyzer",
	irpcDigiTag = cms.InputTag("simMuonIRPCDigis"),
	thrTime = cms.double(0.00001),
	thrStripNum = cms.double(0.9)
)

process.p = cms.Path(process.demo)
