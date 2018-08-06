import FWCore.ParameterSet.Config as cms
from MiniTree.Selection.LocalRunSkeleton_cff import *
from MiniTree.Selection.LocalSources_cff import toPrint

#-----------------------------
#INPUT & OUTPUT
#-----------------------------
isData=False
#isData=True
#Data
#inFile = "/store/data/Run2016B/SingleMuon/MINIAOD/23Sep2016-v3/00000/00AE0629-1F98-E611-921A-008CFA1112CC.root"
#MC
####inFile ="/store/data/Run2016C/DoubleEG/MINIAOD/03Feb2017-v1/80000/523A8E99-85EC-E611-AAD7-02163E01A316.root" 
#####inFile = "/store/user/sthakur/LHEGeneration_Ele/MiniAODGenerationStepMiniAOD_Ele2000_2018_4_20/180420_120233/0000/mini_AODSIM_Ele2000_1.root"
##inFile = "file:mini_AODSIM_Ele2000_1.root"
#inFile = "/store/user/sthakur/LHEGeneration_Ele/MiniAODGenerationStepMiniAOD_Ele2000_2018_4_20/180420_120233/0000/mini_AODSIM_Ele2000_11.root"
####inFile = "/store/user/sthakur/LHEGeneration_Ele/MiniAODGenerationStepMiniAOD_Ele4000_2018_4_20/180420_122034/0000/mini_AODSIM_Ele4000_1.root"
###inFile = "/store/user/sthakur/LHEGeneration_Ele/MiniAODGenerationStepMiniAOD_Ele6000_2018_4_20/180420_140017/0000/mini_AODSIM_Ele6000_1.root"
##inFile = "/store/mc/RunIISummer16MiniAODv2/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext1-v2/120000/0EA60289-18C4-E611-8A8F-008CFA110AB4.root"
###inFile = "/store/mc/RunIISummer16MiniAODv2/TT_TuneCUETP8M2T4_13TeV-powheg-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/50000/0693E0E7-97BE-E611-B32F-0CC47A78A3D8.root"
inFile = "/store/user/sthakur/TestGenSim_Mu/MiniAODGenerationStepMiniAOD_MuMuZ-250_2018_08_5/180805_043130/0000/EXO-RunIISummer16MiniAODv2-10637_9.root"
##inFile = "root://cms-xrd-global.cern.ch//store/mc/RunIISummer16MiniAODv2/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext2-v1/90000/F6D062B0-14F1-E611-B7BA-02163E0141C2.root"
#inFile = "file:0693E0E7-97BE-E611-B32F-0CC47A78A3D8.root"
######inFile = "file:inFile_"
#inFile = "file:local_run/ele_sig_2000_all_miniaod.root"

process.source.fileNames = [inFile]
process.maxEvents.input = cms.untracked.int32(1000)
##process.maxEvents.input = cms.untracked.int32(-1)

#Savdhan: for multi CRAB
process.TFileService.fileName = cms.string("outFile_.root")
#process.TFileService.fileName = cms.string("mini_AODSIM_Ele2000_134.root")
#process.TFileService.fileName = cms.string("ntuple_TestLHEGeneration_Ele_6000.root")

#-----------------------------
#CONFIG PARAMETERS 
#-----------------------------
procName='LOCALUSER'
#trigMenu = 'HLT2' #https://twiki.cern.ch/twiki/bin/view/CMSPublic/WorkBookMiniAOD
trigMenu = 'HLT'
isFastsim = False
trigpath = ''

#-----------------------------
#START PROCESS CONFIGURATION 
#-----------------------------
process.setName_(procName)
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff")
process.GlobalTag.globaltag  = cms.string('80X_mcRun2_asymptotic_2016_TrancheIV_v6')

#-----------------------------
#CONFIGURE THE EXTRA MODULE 
#-----------------------------
storeOutPath=False
producePDFweights=False
defineBasePreSelection(process,False, False)

#-----------------------------
# ADD THE ANALYSIS MODULE 
#-----------------------------
process.load('MiniTree.Selection.selection_cfi')
process.myMiniTreeProducer.MCTruth.isData = cms.bool(isData)
if isData:
    process.myMiniTreeProducer.MCTruth.sampleCode = cms.string("DATA")
else:
    #Savdhan: for multi CRAB
    process.myMiniTreeProducer.MCTruth.sampleCode = cms.string("sampCode_")
    ##process.myMiniTreeProducer.MCTruth.sampleCode = cms.string("TestLHEGeneration_Ele_2000")
process.myMiniTreeProducer.MCTruth.producePDFweights = cms.bool(producePDFweights)
#process.myMiniTreeProducer.minEventQualityToStore = cms.int32(1)
process.myMiniTreeProducer.Trigger.source = cms.InputTag('TriggerResults::'+trigMenu)

## Savdhan: ONLY FOR SIGNAL MC
#process.myMiniTreeProducer.Trigger.myTrig = "HLT_"
## -------------------
##process.myMiniTreeProducer.Trigger.myTrig = "HLT_DoubleEle33_CaloIdL"

process.myMiniTreeProducer.MCTruth.sampleChannel = cms.string('electron')
#https://github.com/cms-jet/JRDatabase/tree/master/textFiles/Spring16_25nsV10_MC
process.myMiniTreeProducer.Jets.resolutionsFile = cms.string('Spring16_25nsV10_MC_PtResolution_AK8PF.txt')
process.myMiniTreeProducer.Jets.scaleFactorsFile = cms.string('Spring16_25nsV10_MC_SF_AK8PF.txt')

#-----------------------------
#ANALYSIS SEQUENCE 
#-----------------------------
process.p  = cms.Path(process.allEventsFilter*process.basePreSel*process.myMiniTreeProducer)
process.schedule = cms.Schedule(process.p)
checkProcessSchedule(storeOutPath,True)

