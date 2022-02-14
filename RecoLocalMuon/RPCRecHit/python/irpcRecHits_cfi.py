
import FWCore.ParameterSet.Config as cms

#rpcRecHits = cms.EDProducer("RPCRecHitProducer",
#    recAlgoConfig = cms.PSet(

#    ),
#    recAlgo = cms.string('RPCRecHitStandardAlgo'),
#    rpcDigiLabel = cms.InputTag('muonRPCDigis'),
#    maskSource = cms.string('File'),
#    maskvecfile = cms.FileInPath('RecoLocalMuon/RPCRecHit/data/RPCMaskVec.dat'),
#    deadSource = cms.string('File'),
#    deadvecfile = cms.FileInPath('RecoLocalMuon/RPCRecHit/data/RPCDeadVec.dat')
#)
##############################
irpcRecHits = cms.EDProducer("RPCRecHitProducer",
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
               rpcDigiLabel = cms.InputTag('muonRPCDigis'),
               maskSource = cms.string('File'),
               maskvecfile = cms.FileInPath('RecoLocalMuon/RPCRecHit/data/RPCMaskVec.dat'),
               deadSource = cms.string('File'),
               deadvecfile = cms.FileInPath('RecoLocalMuon/RPCRecHit/data/RPCDeadVec.dat')
             )

##############################
#disabling DIGI2RAW,RAW2DIGI chain for Phase2
from Configuration.Eras.Modifier_phase2_muon_cff import phase2_muon
phase2_muon.toModify(irpcRecHits, rpcDigiLabel = 'simMuonRPCDigis')
