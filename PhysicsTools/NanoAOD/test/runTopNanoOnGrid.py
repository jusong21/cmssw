#! /usr/bin/env python

from CRABAPI.RawCommand import crabCommand
try:
    import CRABClient.UserUtilities as crab
except ImportError as e:
    print("Could not import CRAB API, have you run 'source /cvmfs/cms.cern.ch/common/crab-setup.sh'?")
    raise e

import yaml
import copy
import os
import argparse
import sys

# DAS client
from Utilities.General.cmssw_das_client import get_data as myDASclient

CMSSW_ROOT = os.path.join(os.environ['CMSSW_BASE'], 'src')
NANO_ROOT = os.path.join(os.environ['CMSSW_BASE'], 'src', 'PhysicsTools', 'NanoAOD')
PROD_TAG = "v9-1-1"

def retry(nattempts, exception=None):
    """
    Decorator allowing to retry an action several times before giving up.
    @params:
        nattempts  - Required: maximal number of attempts (Int)
        exception  - Optional: if given, only catch this exception, otherwise catch 'em all (Exception)
    """

    def tryIt(func):
        def wrapper(*args, **kwargs):
            attempts = 0
            while attempts < nattempts - 1:
                try:
                    return func(*args, **kwargs)
                except (exception if exception is not None else Exception):
                    attempts += 1
            return func(*args, **kwargs)
        return wrapper
    return tryIt

@retry(5)
def get_parent_DAS(dataset):
    """Retrieve parent dataset from DAS"""
    data = myDASclient("parent dataset=" + dataset)
    if data['status'] != 'ok':
        raise Exception("Failed retrieving parent dataset from DAS.\n{}".format(data))
    assert(len(data['data']) == 1)
    assert(len(data['data'][0]['parent']) == 1)
    return data['data'][0]['parent'][0]['name']


def get_options():
    """
    Parse and return the arguments provided by the user.
    """
    parser = argparse.ArgumentParser(description='Generate crab config files for multiple datasets.')

    parser.add_argument('-e', '--era', required=False, choices=['2016', '2016ULpreVFP', '2016ULpostVFP', '2017UL', '2018UL'], help='Choose specific era. If not specified, run on all eras')
    parser.add_argument('-d', '--datasets', nargs='*', help='YAML file(s) with dataset list')
    parser.add_argument('-s', '--site', required=True, help='Grid site to which to write the output (you NEED write permission on that site!)')
    parser.add_argument('-o', '--output', default='./', help='Folder in which to write the crab config files')
    parser.add_argument('--rucio', action='store_true', help='Publish the output to Rucio INSTEAD of DBS. Requires a Rucio quota at the stageout site!')

    return parser.parse_args()


def create_default_config(is_mc):
    config = crab.config()

    config.General.workArea = 'tasks'
    config.General.transferOutputs = True
    config.General.transferLogs = True
    config.JobType.allowUndistributedCMSSW = True # for slc7

    config.JobType.pluginName = 'Analysis'
    config.JobType.maxMemoryMB = 3000
    config.JobType.numCores = 2

    config.Data.inputDBS = 'global'

    config.Data.splitting = 'FileBased'
    config.Data.unitsPerJob = 1
    # if is_mc:
    #     config.Data.splitting = 'EventAwareLumiBased'
    #     config.Data.unitsPerJob = 280000
    # else:
    #     config.Data.splitting = 'LumiBased'
    #     config.Data.unitsPerJob = 220

    return config


def findPSet(pset):
    c = pset
    if not os.path.isfile(c):
        # Try to find the psetName file
        filename = os.path.basename(c)
        path = NANO_ROOT
        c = None
        for root, dirs, files in os.walk(path):
            if filename in files:
                c = os.path.join(root, filename)
                break
        if c is None:
            raise IOError('Configuration file %r not found' % filename)
    return os.path.abspath(c)


def writeCrabConfig(pset, dataset, is_mc, name, metadata, era, crab_config, site, output, use_rucio=False):
    c = copy.deepcopy(crab_config)

    c.JobType.psetName = pset

    c.General.requestName = "TopNanoAOD{}_{}__{}".format(PROD_TAG, name, era)
    c.Data.inputDataset = dataset

    if use_rucio:
        # see https://twiki.cern.ch/twiki/bin/view/CMSPublic/RucioUserDocsData
        prefix = '/store/user/rucio/'
        # do not publish in DBS (for now)
        c.Data.publication = False
        # include dataset name in the tag
        c.Data.outputDatasetTag = "{}_TopNanoAOD{}_{}".format(name, PROD_TAG, era)
    else:
        # publish in DBS
        c.Data.publication = True
        c.Data.outputDatasetTag = "TopNanoAOD{}_{}".format(PROD_TAG, era)
        prefix = '/store/user/'
    c.Data.outLFNDirBase = prefix + '{user}/topNanoAOD/{tag}/{era}/'.format(user=os.getenv('USER'), tag=PROD_TAG, era=era)
    c.Site.storageSite = site

    # customize if asked
    for attr,val in metadata.items():
        setattr(getattr(c, attr.split(".")[0]), attr.split(".")[1], val)

    print("Creating new task {}".format(c.General.requestName))

    # Create output file
    crab_config_file = os.path.join(output, 'crab_' + c.General.requestName + '.py')
    with open(crab_config_file, 'w') as f:
        f.write(str(c))

    print('Configuration file saved as %r' % (crab_config_file))



if __name__ == "__main__":

    options = get_options()
    
    if not os.path.isdir(options.output):
        os.makedirs(options.output)

    # Load datasets
    datasets = {}
    for dataset in options.datasets:
        with open(dataset) as f:
            datasets.update(yaml.safe_load(f))

    crab_config_mc = create_default_config(True)
    crab_config_data = create_default_config(False)

    for era, era_datasets in datasets.items():
        if options.era and era != options.era:
            continue

        for name,metadata in era_datasets.items():
            dataset = metadata.pop("das")
            print("Working on {}".format(dataset))
            if dataset.endswith("NANOAODSIM") or dataset.endswith("NANOAOD"):
                print("Will convert from nano to mini!")
                dataset = get_parent_DAS(dataset)
                print(" --> Found {}".format(dataset))
            elif (not (dataset.endswith("MINIAODSIM") or dataset.endswith("MINIAOD"))) and not dataset.endswith("USER"):
                print("Dataset {} cannot be used - must be either nano or mini!".format(dataset))

            is_mc = dataset.endswith("SIM")
            if is_mc:
                print("Dataset is MC")
                pset = findPSet("topNano_{}_{}_MC_cfg.py".format(PROD_TAG, era))
                crab_config = crab_config_mc
            else:
                print("Dataset is data")
                crab_config = crab_config_data
                pset = findPSet("topNano_{}_{}_data_cfg.py".format(PROD_TAG, era))
                # make sure from now on we keep the "actual" (letter) era for data:
                era = dataset.split("/")[2].split("_")[0] # e.g. Run2016H-2016UL

            writeCrabConfig(pset, dataset, is_mc, name, metadata, era, crab_config, options.site, options.output, options.rucio)
            print("")
