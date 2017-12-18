# MiniTree

#### Set the CMSSSW release ####

* cmsrel CMSSW_8_0_25
* cd CMSSW_8_0_25/src/
* cmsenv

#### Download the MiniTree package ####

* git clone https://github.com/ravindkv/MiniTree.git

#### Compile and Run the codes ####

* cd MiniTree 
* scram b -j20
* cd Selection/test
* cmsRun muons_miniAOD_to_ntuple_13TeV_cfg.py

