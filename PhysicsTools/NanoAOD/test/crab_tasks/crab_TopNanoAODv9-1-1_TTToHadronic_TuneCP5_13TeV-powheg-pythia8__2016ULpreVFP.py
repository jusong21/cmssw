from WMCore.Configuration import Configuration
config = Configuration()
config.section_('General')
config.General.workArea = 'tasks'
config.General.transferOutputs = True
config.General.transferLogs = True
config.General.requestName = 'TopNanoAODv9-1-1_TTToHadronic_TuneCP5_13TeV-powheg-pythia8__2016ULpreVFP'
config.section_('JobType')
config.JobType.psetName = '/ada_mnt/ada/user/jusong/analysis/CMSSW_10_6_27/src/PhysicsTools/NanoAOD/test/topNano_v9-1-1_2016ULpreVFP_MC_cfg.py'
config.JobType.pluginName = 'Analysis'
config.JobType.numCores = 2
config.JobType.allowUndistributedCMSSW = True
config.JobType.maxMemoryMB = 3000
config.section_('Data')
config.Data.inputDataset = '/TTToHadronic_TuneCP5_13TeV-powheg-pythia8/RunIISummer20UL16MiniAODAPVv2-106X_mcRun2_asymptotic_preVFP_v11-v1/MINIAODSIM'
config.Data.outputDatasetTag = 'TopNanoAODv9-1-1_2016ULpreVFP'
config.Data.publication = True
config.Data.unitsPerJob = 1
config.Data.inputDBS = 'global'
config.Data.splitting = 'FileBased'
config.Data.outLFNDirBase = '/store/user/jusong/topNanoAOD/v9-1-1/2016ULpreVFP/'
config.section_('Site')
config.Site.storageSite = 'T2_BE_IIHE'
config.section_('User')
config.section_('Debug')
