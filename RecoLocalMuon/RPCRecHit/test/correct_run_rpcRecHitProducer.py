import FWCore.ParameterSet.Config as cms
import os

process = cms.Process('ReDoRecHit')

process.load('Configuration.Geometry.GeometryExtended2026D86Reco_cff')
process.load('Configuration.StandardSequences.MagneticField_cff')
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:phase2_realistic_T21', '')

process.maxEvents = cms.untracked.PSet(
                        input = cms.untracked.int32(-1),
                    )

#from RecoLocalMuon.RPCRecHit.rpcRecHits_cfi import rpcRecHits

process.irpcRecHits = cms.EDProducer("RPCRecHitProducer",
               recAlgoConfig = cms.PSet(
                                 iRPCConfig = cms.PSet( # iRPC Algorithm
                                   useAlgorithm = cms.bool(True), # useIRPCAlgorithm: if true - use iRPC Algorithm;
                                   returnOnlyAND = cms.bool(True), # returnOnlyAND: if true algorithm will return only associated clusters;
                                   returnOnlyHR = cms.bool(False), # returnOnlyHR: if true algorithm will return only HR cluster
                                   returnOnlyLR = cms.bool(False), # returnOnlyLR: if true algorithm will return only LR clusters;
                                   thrTimeHR = cms.double(2), # [ns] thrTimeHR: threshold for high radius clustering;
                                   thrTimeLR = cms.double(2), # [ns] thrTimeLR: threshold for low radius clustering;
                                   thrDeltaTimeMin = cms.double(-30), # [ns] thrDeltaTimeMin: min delta time threshold for association clusters between HR and LR;
                                   thrDeltaTimeMax = cms.double(30), # [ns] thrDeltaTimeMax: max delta time threshold for association clusters between HR and LR;
                                   thrDeltaTimeY = cms.double(0.2), # [ns] thrDeltaTimeY: position uncertainty;
                                   signalSpeed = cms.double(19.786302) # [cm/ns] signalSpeed: speed of signal inside strip.
                                 ),
                               ),
               recAlgo = cms.string('RPCRecHitStandardAlgo'),
               rpcDigiLabel = cms.InputTag('simMuonRPCDigis'),
               maskSource = cms.string('File'),
               maskvecfile = cms.FileInPath('RecoLocalMuon/RPCRecHit/data/RPCMaskVec.dat'),
               deadSource = cms.string('File'),
               deadvecfile = cms.FileInPath('RecoLocalMuon/RPCRecHit/data/RPCDeadVec.dat')
             )

#No need to redo the normal recHits since they are already in the input rootfile
process.rpcRecHits = cms.EDProducer("RPCRecHitProducer",
               recAlgoConfig = cms.PSet(
                                 iRPCConfig = cms.PSet( # iRPC Algorithm
                                   useAlgorithm = cms.bool(False), # useIRPCAlgorithm: if true - use iRPC Algorithm;
                                   returnOnlyAND = cms.bool(True), # returnOnlyAND: if true algorithm will return only associated clusters;
                                   returnOnlyHR = cms.bool(False), # returnOnlyHR: if true algorithm will return only HR cluster
                                   returnOnlyLR = cms.bool(False), # returnOnlyLR: if true algorithm will return only LR clusters;
                                   thrTimeHR = cms.double(2), # [ns] thrTimeHR: threshold for high radius clustering;
                                   thrTimeLR = cms.double(2), # [ns] thrTimeLR: threshold for low radius clustering;
                                   thrDeltaTimeMin = cms.double(-30), # [ns] thrDeltaTimeMin: min delta time threshold for association clusters between HR and LR;
                                   thrDeltaTimeMax = cms.double(30), # [ns] thrDeltaTimeMax: max delta time threshold for association clusters between HR and LR;
                                   thrDeltaTimeY = cms.double(0.2), # [ns] thrDeltaTimeY: position uncertainty;
                                   signalSpeed = cms.double(19.786302) # [cm/ns] signalSpeed: speed of signal inside strip.
                                 ),
                               ),
               recAlgo = cms.string('RPCRecHitStandardAlgo'),
               rpcDigiLabel = cms.InputTag('simMuonRPCDigis'),
               maskSource = cms.string('File'),
               maskvecfile = cms.FileInPath('RecoLocalMuon/RPCRecHit/data/RPCMaskVec.dat'),
               deadSource = cms.string('File'),
               deadvecfile = cms.FileInPath('RecoLocalMuon/RPCRecHit/data/RPCDeadVec.dat')
             )

process.source = cms.Source('PoolSource',
                  fileNames = cms.untracked.vstring(
'file:/afs/cern.ch/work/m/mileva/triggerStudies/iRPCClusters/testFeb2022/CMSSW_12_2_0_pre3/src/step2_digiOnly_standard.root'
#'file:/afs/cern.ch/work/m/mileva/triggerStudies/iRPCClusters/testFeb2022/CMSSW_12_2_0_pre3/src/step2_3_digi2reco_standard.root'

)
)
 
# Output
process.out = cms.OutputModule("PoolOutputModule",
                fileName = cms.untracked.string('MC_iRPC_RecHit.root'),
                outputCommands = cms.untracked.vstring(
                    'drop *_*_*_*',
                    'keep *_standAloneMuons_*_*',
                    'keep *RPC*_*_*_*',
                    'keep *_csc*_*_*',
                    'keep *_dt*_*_*',
                    'keep *_gem*_*_*',
                    'keep *_rpc*_*_*'
)
              )

process.p = cms.Path(process.rpcRecHits + process.irpcRecHits)
#process.p = cms.Path(process.irpcRecHits)
process.e = cms.EndPath(process.out)
