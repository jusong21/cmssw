# no shebang, this should be sourced

PROD_TAG=v9-1-1

# global tags and eras from https://gitlab.cern.ch/cms-nanoAOD/nanoaod-doc/-/wikis/Releases/NanoAODv9

function runDriverMC () {
    cmsDriver.py NANO --python_filename topNano_${PROD_TAG}_$1_MC_cfg.py --fileout file:tree.root -s NANO --mc --conditions $2 --era $3 --eventcontent NANOAODSIM --datatier NANOAODSIM --customise_commands="process.add_(cms.Service('InitRootHandlers', EnableIMT = cms.untracked.bool(False)));process.MessageLogger.cerr.FwkReport.reportEvery=1000;process.NANOAODSIMoutput.fakeNameForCrab=cms.untracked.bool(True)" --nThreads 2 -n -1 --no_exec
}

runDriverMC 2016ULpreVFP  106X_mcRun2_asymptotic_preVFP_v11      Run2_2016,run2_nanoAOD_106Xv2
runDriverMC 2016ULpostVFP 106X_mcRun2_asymptotic_v17             Run2_2016,run2_nanoAOD_106Xv2
runDriverMC 2017UL        106X_mc2017_realistic_v9               Run2_2017,run2_nanoAOD_106Xv2
runDriverMC 2018UL        106X_upgrade2018_realistic_v16_L1v1    Run2_2018,run2_nanoAOD_106Xv2

function runDriverData () {
    cmsDriver.py NANO  --python_filename topNano_${PROD_TAG}_$1_data_cfg.py --fileout file:tree.root -s NANO --data --conditions $2 --era $3 --eventcontent NANOAOD --datatier NANOAOD --customise_commands="process.add_(cms.Service('InitRootHandlers', EnableIMT = cms.untracked.bool(False)));process.MessageLogger.cerr.FwkReport.reportEvery=1000;process.NANOAODoutput.fakeNameForCrab=cms.untracked.bool(True)" --nThreads 2 -n -1 --no_exec
}

runDriverData 2016UL    106X_dataRun2_v35   Run2_2016,run2_nanoAOD_106Xv2
runDriverData 2017UL    106X_dataRun2_v35   Run2_2017,run2_nanoAOD_106Xv2
runDriverData 2018UL    106X_dataRun2_v35   Run2_2018,run2_nanoAOD_106Xv2
