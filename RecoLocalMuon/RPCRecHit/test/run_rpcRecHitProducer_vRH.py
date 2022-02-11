import FWCore.ParameterSet.Config as cms
#import os
#from optparse import OptionParser
#import sys
#import imp
#parser = OptionParser()
#parser.usage = "%prog <file> : expand this python configuration"

#(options,args) = parser.parse_args()

#inDeltaTimeY = 1
#inThrHR = 1
#inThrLR = 1
#inDeltaTimeY = args[1]
#inThrHR = args[2]
#inThrLR = args[3]

process = cms.Process('ReDoRecHit')

#process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
#process.GlobalTag.globaltag = "106X_upgrade2018_realistic_v5"  #4"
##should be v5 and not v4
##in order to avoid, load it using aliases :

from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:phase2_realistic', '')


process.maxEvents = cms.untracked.PSet(
                        input = cms.untracked.int32(100),
                    )
#process.load('Configuration.Geometry.GeometryExtended2023D38Reco_cff')
#process.load('Configuration.Geometry.GeometryExtended2023D38_cff')
process.load('Configuration.Geometry.GeometryExtended2023D41Reco_cff')
process.load('Configuration.Geometry.GeometryExtended2023D41_cff')


#from RecoLocalMuon.RPCRecHit.rpcRecHits_cfi import rpcRecHits

process.irpcRecHits = cms.EDProducer("RPCRecHitProducer",
               recAlgoConfig = cms.PSet(
                                 iRPCConfig = cms.PSet( # iRPC Algorithm
                                   useAlgorithm = cms.bool(True), # useIRPCAlgorithm: if true - use iRPC Algorithm;
                                   returnOnlyAND = cms.bool(True), # returnOnlyAND: if true algorithm will return only associated clusters;
                                   returnOnlyHR = cms.bool(False), # returnOnlyHR: if true algorithm will return only HR clusters;
                                   returnOnlyLR = cms.bool(False), # returnOnlyLR: if true algorithm will return only LR clusters;
#                                   thrTimeHR = cms.double(float(inThrHR)), # [ns] thrTimeHR: threshold for high radius clustering;
#                                   thrTimeLR = cms.double(float(inThrLR)), # [ns] thrTimeLR: threshold for low radius clustering;
                                   thrTimeHR = cms.double(1), # [ns] thrTimeHR: threshold for high radius clustering;
                                   thrTimeLR = cms.double(1), # [ns] thrTimeLR: threshold for low radius clustering;
                                   thrDeltaTimeMin = cms.double(-200), # [ns] thrDeltaTimeMin: min delta time threshold for association clusters between HR and LR;
                                   thrDeltaTimeMax = cms.double(200), # [ns] thrDeltaTimeMax: max delta time threshold for association clusters between HR and LR;
                                   thrDeltaTimeY = cms.double(1), # [ns] thrDeltaTimeY: position uncertainty;
#                                   thrDeltaTimeY = cms.double(float(inDeltaTimeY)), # [ns] thrDeltaTimeY: position uncertainty;
                                   signalSpeed = cms.double(19.786302) # [cm/ns] signalSpeed: speed of signal inside strip.

                                   #useAlgorithm = cms.bool(True), # useIRPCAlgorithm: if true - use iRPC Algorithm;
                                   #returnOnlyAND = cms.bool(True), # returnOnlyAND: if true algorithm will return only associated clusters;
                                   #thrTimeHR = cms.double(2), # [ns] thrTimeHR: threshold for high radius clustering;
                                   #thrTimeLR = cms.double(2), # [ns] thrTimeLR: threshold for low radius clustering;
                                   #thrDeltaTimeMin = cms.double(-30), # [ns] thrDeltaTimeMin: min delta time threshold for association clusters between HR and LR;
                                   #thrDeltaTimeMax = cms.double(30), # [ns] thrDeltaTimeMax: max delta time threshold for association clusters between HR and LR;
                                   #signalSpeed = cms.double(19.786302) # [cm/ns] signalSpeed: speed of signal inside strip.
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
#process.rpcRecHits = cms.EDProducer("RPCRecHitProducer",
#               recAlgoConfig = cms.PSet(
#                                 iRPCConfig = cms.PSet( # iRPC Algorithm
#                                   useAlgorithm = cms.bool(False), # useIRPCAlgorithm: if true - use iRPC Algorithm;
#                                   returnOnlyAND = cms.bool(True), # returnOnlyAND: if true algorithm will return only associated clusters;
#                                   thrTimeHR = cms.double(2), # [ns] thrTimeHR: threshold for high radius clustering;
#                                   thrTimeLR = cms.double(2), # [ns] thrTimeLR: threshold for low radius clustering;
#                                   thrDeltaTimeMin = cms.double(-30), # [ns] thrDeltaTimeMin: min delta time threshold for association clusters between HR and LR;
#                                   thrDeltaTimeMax = cms.double(30), # [ns] thrDeltaTimeMax: max delta time threshold for association clusters between HR and LR;
#                                   signalSpeed = cms.double(19.786302) # [cm/ns] signalSpeed: speed of signal inside strip.
#                                 ),
#                               ),
#               recAlgo = cms.string('RPCRecHitStandardAlgo'),
#               rpcDigiLabel = cms.InputTag('simMuonRPCDigis'),
#               maskSource = cms.string('File'),
#               maskvecfile = cms.FileInPath('RecoLocalMuon/RPCRecHit/data/RPCMaskVec.dat'),
#               deadSource = cms.string('File'),
#               deadvecfile = cms.FileInPath('RecoLocalMuon/RPCRecHit/data/RPCDeadVec.dat')
#             )

# Input source
##input_files = cms.untracked.vstring()
#inputPath = "/eos/cms/store/group/dpg_rpc/comm_rpc/UpgradePhaseII/iRPCClustering/SingleMu/reco_iRPConeRollSingleMu100_200k_v2/190810_102015/0000/"

#inputPath = "/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/0PU/0000/"
##inputPath = "/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/"
##for input_file in os.listdir(inputPath):
##    if "_" in input_file:
##        input_files.append("file:" + os.path.join(inputPath, input_file))
##print input_files
process.source = cms.Source('PoolSource',
#                fileNames = input_files
                fileNames = cms.untracked.vstring(
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_1.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_10.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_100.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_101.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_102.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_103.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_104.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_105.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_106.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_107.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_108.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_109.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_11.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_110.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_111.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_112.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_113.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_114.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_115.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_116.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_117.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_118.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_119.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_12.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_120.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_121.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_122.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_123.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_124.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_125.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_126.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_127.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_128.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_129.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_13.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_130.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_131.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_132.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_133.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_134.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_135.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_136.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_137.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_138.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_139.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_14.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_140.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_141.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_142.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_143.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_144.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_145.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_146.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_147.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_148.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_149.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_15.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_150.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_151.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_152.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_153.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_154.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_155.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_156.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_157.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_158.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_159.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_16.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_160.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_161.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_162.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_163.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_164.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_165.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_166.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_167.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_168.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_169.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_17.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_170.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_171.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_172.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_173.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_174.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_175.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_176.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_177.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_178.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_179.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_18.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_180.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_181.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_182.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_183.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_184.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_185.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_186.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_187.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_188.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_189.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_19.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_190.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_191.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_192.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_193.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_194.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_195.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_196.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_197.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_198.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_199.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_2.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_20.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_200.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_201.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_202.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_203.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_204.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_205.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_206.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_207.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_208.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_209.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_21.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_210.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_211.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_212.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_213.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_214.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_215.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_216.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_217.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_218.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_219.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_22.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_220.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_221.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_222.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_223.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_224.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_225.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_226.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_227.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_228.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_229.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_23.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_230.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_231.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_232.root',
'file:/eos/cms/store/group/dpg_rpc/comm_rpc/Trigger/L1TDR_Samples_WithMuonReco/Mu_FlatPt2to100-pythia8-gun/Mu_FlatPt2to100-pythia8-gun_withMuonReco/200PU/0000/Mu_FlatPt2to100-pythia8-gun_FFCFF986-ED0B-B74F-B253-C511D19B8249_withLocalMuonReco_233.root'
)
                 )
# Output
process.out = cms.OutputModule("PoolOutputModule",
                fileName =
#                cms.untracked.string('/afs/cern.ch/work/k/kshchabl/public/residual/MC_RPC_RecHit' + '.root'),
                cms.untracked.string('outputTest.root'),

                outputCommands = cms.untracked.vstring(
                    'drop *_*_*_*',
                    'keep *_standAloneMuons_*_*',
                    'keep *RPC*_*_*_*',
                    'keep *_csc*_*_*',
                    'keep *_dt*_*_*',
                    'keep *_gem*_*_*',
                    'keep *_me0*_*_*'
)
              )

#process.p = cms.Path(process.rpcRecHits + process.irpcRecHits)
process.p = cms.Path(process.irpcRecHits)
process.e = cms.EndPath(process.out)
