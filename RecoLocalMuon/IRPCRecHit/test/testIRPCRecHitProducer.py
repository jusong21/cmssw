import FWCore.ParameterSet.Config as cms
from Configuration.StandardSequences.GeometryDB_cff import *
from Configuration.StandardSequences.MagneticField_cff import *
from Configuration.StandardSequences.FrontierConditions_GlobalTag_cff import *

process = cms.Process("RECLUSTERIZATION")

process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")

from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, "auto:phase2_realistic_T25", "")

process.load("Geometry.MuonCommonData.muonIdealGeometryXML_cfi")
#process.load("Geometry.RPCGeometry.rpcGeometry_cfi")
process.load("Geometry.RPCGeometryBuilder.rpcGeometry_cfi")
process.load("Geometry.MuonNumbering.muonNumberingInitialization_cfi")

#process.load("Configuration.Geometry.GeometryIdeal_cff")  # Geometry configuration
# for RPC Geometry
process.load("Configuration.StandardSequences.GeometryRecoDB_cff")
#process.load("Configuration.StandardSequences.GeometryDD4hepExtended2026D98Reco_cff")



process.load("RecoLocalMuon.IRPCRecHit.irpcRecHits_cfi")

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(30000)
)

process.source = cms.Source("PoolSource",
                            #    debugFlag = cms.untracked.bool(True),
                            #    debugVebosity = cms.untracked.uint32(10),
#                            fileNames = cms.untracked.vstring(
#        "file:/eos/home-j/jusong/work/iRPC/CMSSW_15_1_0_pre3/src/IRPC/step2.root"
        fileNames = cms.untracked.vstring('file:/eos/home-j/jusong/work/iRPC/CMSSW_15_1_0_pre3/src/IRPC/step2.root')
)

process.out = cms.OutputModule("PoolOutputModule",
    fileName = cms.untracked.string('file:rechit01.root')
)

process.p = cms.Path(process.irpcRecHits)
process.ep = cms.EndPath(process.out)



process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.debugModules = cms.untracked.vstring("*")
process.MessageLogger.cerr.enable = False
process.MessageLogger.files.junk = dict()
process.MessageLogger.cout = cms.untracked.PSet(
    enable = cms.untracked.bool(True),
    threshold = cms.untracked.string("DEBUG"),
    default = cms.untracked.PSet( limit = cms.untracked.int32(0) ),
    # Add your RPCDump category here
    RPCDump = cms.untracked.PSet( limit = cms.untracked.int32(10000000) )
)
