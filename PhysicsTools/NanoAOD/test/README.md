# TopNanoAOD

## Recipe: setting up (v9-1-X)

Based on central nanoAOD v9, CMSSW_10_6_27. On CC7/SLC7:
```bash
export SCRAM_ARCH=slc7_amd64_gcc700
cmsrel CMSSW_10_6_27
cd CMSSW_10_6_27/src/
cmsenv
git cms-init
git cms-checkout-topic cms-top:topNanoV9-1-1_10_6_27
scram b -j 8
```

## Submitting crab jobs

```bash
voms-proxy-init --rfc --voms cms --valid 96:00
source /cvmfs/cms.cern.ch/common/crab-setup.sh # or .csh
cd $CMSSW_BASE/src/PhysicsTools/NanoAOD/test
```

You'll need to prepare a YAML file listing all the samples you want to process. An example is given in [test/topSamples.yml](./topSamples.yml). You can either use the name of the miniAOD dataset you want to process, or the name of the nanoAOD dataset you want to reproduce (the script will then automatically fetch the parent miniAOD dataset). Then, run:
```bash
./runTopNanoOnGrid.py -d YOURLIST.yml -o crab_tasks -s YOURSITE
```
Replace `YOURSITE` by a T2/T3 site at which you have write permissions. This creates the crab config files in the `crab_tasks` folder, but doesnt submit any jobs. You can then `cd crab_tasks` and submit all or some of them using `crab submit PY_CFG`.

After the processing is complete, you can retrieve the output dataset name using `crab status TASK_DIR`, and add it to the repository holding all the topNanoAOD datasets: https://github.com/cms-top/topNanoAOD-datasets/

**Important**: the Crab files included here are configured in such a way that the output datasets will be known to [Rucio](https://twiki.cern.ch/twiki/bin/view/CMS/Rucio), the new sample management tool, so that people are able to request copies of the topNanoAOD outputs to their local computing centre using Rucio.

## Troubleshooting

You might need to adjust the job splitting options if jobs take too long to run (or run too quickly). Resubmitting using `crab resubmit --maxjobruntime` and `--siteblacklist` can also help.

In case you accidentally published a dataset twice under the same publication name, one can invalidate specific files of the dataset:
```bash
wget -O DBS3SetFileStatus.py https://twiki.cern.ch/twiki/pub/CMSPublic/Crab3DataHandling/DBS3SetFileStatus.py.txt
python DBS3SetFileStatus.py --url=https://cmsweb.cern.ch/dbs/prod/phys03/DBSWriter --status=invalid --recursive=False  --files=<LFN>
```
To invalidate a complete dataset, use this command:
```bash
wget -O DBS3SetDatasetStatus.py https://twiki.cern.ch/twiki/pub/CMSPublic/Crab3DataHandling/DBS3SetDatasetStatus.py.txt
python DBS3SetDatasetStatus.py --dataset=<datasetname> --url=https://cmsweb.cern.ch/dbs/prod/phys03/DBSWriter --status=INVALID --recursive=False
```
More information about publication [here](https://twiki.cern.ch/twiki/bin/view/CMSPublic/Crab3DataHandling#Changing_a_dataset_or_file_statu).

## Version history

### nanoAODv9

- **v9-1-1**:
    - Store full set of PS weights
    - Origin of B and C hadrons ghost-matched to genJets
    - Crab submission scripts: enable Rucio publication of outputs
    - [Auto-generated documentation](https://swertz.web.cern.ch/TMG/TopNano/TopNanoV9/doc_topNanoV9-1-1_MC18UL.html)
    - [Full code changes w.r.t central nanoAOD](https://github.com/cms-top/cmssw/compare/CMSSW_10_6_27...topNanoV9-1-1_10_6_27)

### nanoAODv6

- **v6-1-2**:
    - Note: no event content change w.r.t v6p1
    - Fix DBS publication for data
- **v6-1-1**:
    - Note: no event content change w.r.t v6p1
    - Add cmsDriver cfg's and adapt crab submission scripts for also running on data
    - Fix typo in README
    - [Full code changes](https://github.com/cms-top/cmssw/compare/CMSSW_10_2_18...topNanoV6-1-1_10_2_18)
- **v6p1**: see detailed report [here](https://indico.cern.ch/event/921985/contributions/3873532/attachments/2043975/3423837/200525_topNanoV6p1.pdf)
    - TOP lepton MVA
    - Store NNPDF3.1 weights when available (instead of PDF4LHC)
    - PS weights storage fixed as in [this PR](https://github.com/cms-nanoAOD/cmssw/pull/506)
    - Origin of B and C hadron ghost-matched to genJets
    - [Auto-generated documentation](https://swertz.web.cern.ch/swertz/TMG/TopNano/TopNanoV6p1/doc_topNanoV6p1.html)
    - [Size report](https://swertz.web.cern.ch/swertz/TMG/TopNano/TopNanoV6p1/report_topNanoV6p1.html)
    - [Full code changes](https://github.com/cms-top/cmssw/compare/CMSSW_10_2_18...topNanoV6p1_10_2_18)


### nanoAODv5

See instructions [here](https://github.com/demuller/privNanoAOD).

## Contributing changes

If you'd like to propose some changes for future iterations, please open an [issue](https://github.com/cms-top/cmssw/issues) to discuss them, and then open a pull request towards the `topNanoV9_from-CMSSW_10_6_27` branch.