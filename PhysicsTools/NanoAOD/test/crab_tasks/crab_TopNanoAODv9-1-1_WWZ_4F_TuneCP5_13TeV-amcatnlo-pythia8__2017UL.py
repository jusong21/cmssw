from WMCore.Configuration import Configuration
config = Configuration()
config.section_('General')
config.General.workArea = 'tasks'
config.General.transferOutputs = True
config.General.transferLogs = True
config.General.requestName = 'TopNanoAODv9-1-1_WWZ_4F_TuneCP5_13TeV-amcatnlo-pythia8__2017UL'
config.section_('JobType')
config.JobType.psetName = '/ada_mnt/ada/user/jusong/analysis/CMSSW_10_6_27/src/PhysicsTools/NanoAOD/test/topNano_v9-1-1_2017UL_MC_cfg.py'
config.JobType.pluginName = 'Analysis'
config.JobType.numCores = 2
config.JobType.allowUndistributedCMSSW = True
config.JobType.maxMemoryMB = 3000
config.section_('Data')
config.Data.inputDataset = '/WWZ_4F_TuneCP5_13TeV-amcatnlo-pythia8/RunIISummer20UL17MiniAODv2-106X_mc2017_realistic_v9_ext1-v2/MINIAODSIM'
config.Data.outputDatasetTag = 'TopNanoAODv9-1-1_2017UL'
config.Data.publication = True
config.Data.unitsPerJob = 1
config.Data.inputDBS = 'global'
config.Data.splitting = 'FileBased'
config.Data.outLFNDirBase = '/store/user/jusong/topNanoAOD/v9-1-1/2017UL/'
config.section_('Site')
config.Site.storageSite = 'T2_BE_IIHE'
config.section_('User')
config.section_('Debug')
