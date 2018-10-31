from CRABClient.UserUtilities import config, getUsernameFromSiteDB
config = config()

#config.General.workArea = 'crab_projects_offiSignalMu'
config.General.workArea = 'crab_projects_offiSignalEle'
config.General.transferOutputs = True
config.General.transferLogs = True

config.JobType.pluginName = 'Analysis'

####For Muon Siganl MC
##config.General.requestName = 'ExcitedLepton_MuMuZ-2000_15August'
##config.JobType.psetName = 'muonNtuple_cfg.py'
#config.Data.inputDataset = '/TestGenSim_Mu/sthakur-MiniAODGenerationStepMiniAOD_MuMuZ-250_2018_08_5-05265a4e675f881e1e171fd06785811b/USER'
##config.Data.inputDataset = '/ExcitedLepton_MuMuZ-250_TuneCUETP8M1_13TeV_pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v2/MINIAODSIM'
##config.Data.inputDataset = '/ExcitedLepton_MuMuZ-1500_TuneCUETP8M1_13TeV-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v3/MINIAODSIM'
##config.Data.inputDataset = '/ExcitedLepton_MuMuZ-4000_TuneCUETP8M1_13TeV-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v3/MINIAODSIM'
##config.Data.inputDataset = '/ExcitedLepton_MuMuZ-2000_TuneCUETP8M1_13TeV-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v2/MINIAODSIM'

####For Electron Signal MC
config.General.requestName = 'ExcitedLepton_EEZ-4000_26October2018'
config.JobType.psetName = 'electronNtuple_cfg.py'
config.Data.inputDataset = '/ExcitedLepton_EEZ-4000_TuneCUETP8M1_13TeV-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v3/MINIAODSIM'

config.Data.inputDBS = 'global'
##config.Data.inputDBS = 'phys03'
config.JobType.inputFiles = ['Spring16_25nsV10_MC_PtResolution_AK8PF.txt', 'Spring16_25nsV10_MC_SF_AK8PF.txt']

config.Data.outputDatasetTag = 'ExictedMuon_Official_MuMuZ-4000_26October2018'
config.Data.splitting = 'FileBased'
config.Data.unitsPerJob = 2
config.Data.outLFNDirBase = '/store/user/%s/' % (getUsernameFromSiteDB())
config.Data.publication = False

config.Site.storageSite ='T2_IN_TIFR' 
