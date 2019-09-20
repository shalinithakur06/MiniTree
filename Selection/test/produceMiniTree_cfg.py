import FWCore.ParameterSet.Config as cms
#------------------------------------------------------
# The process object
#------------------------------------------------------
process = cms.Process('MiniTree')
process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(True))
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 100

isData=False
#------------------------------------------------------
# Input root files and number of events
#------------------------------------------------------
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring("/store/mc/RunIISummer16MiniAODv2/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext2-v1/90000/FE8A7852-66E4-E611-B5D0-002590E7E01A.root")
    #fileNames = cms.untracked.vstring("file:00AE0629-1F98-E611-921A-008CFA1112CC.root")
)
#inFile = "/store/data/Run2016B/SingleMuon/MINIAOD/23Sep2016-v3/00000/00AE0629-1F98-E611-921A-008CFA1112CC.root"
#inFile = "/store/mc/RunIISummer16MiniAODv2/TT_TuneCUETP8M2T4_13TeV-powheg-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/50000/0693E0E7-97BE-E611-B32F-0CC47A78A3D8.root"
#inFile="/store/mc/RunIISummer16MiniAODv2/ExcitedLepton_MuMuZ-250_TuneCUETP8M1_13TeV_pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v2/20000/E8C3C588-0980-E811-A3D5-0242AC130002.root"
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(5000))

#------------------------------------------------------
# Output file
#------------------------------------------------------
process.load("PhysicsTools.UtilAlgos.TFileService_cfi")
process.TFileService.fileName = cms.string("outFile_.root")

#-----------------------------
#CONFIG PARAMETERS 
#-----------------------------
#trigMenu = 'HLT2' #https://twiki.cern.ch/twiki/bin/view/CMSPublic/WorkBookMiniAOD
trigMenu = 'HLT'
trigpath = ''

#-----------------------------
#START PROCESS CONFIGURATION 
#-----------------------------
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff")
process.GlobalTag.globaltag  = cms.string('80X_mcRun2_asymptotic_2016_TrancheIV_v6')

#-----------------------------
# ADD THE ANALYSIS MODULE 
#-----------------------------
process.load('MiniTree.Selection.EventSelectors_cfi')
process.myMiniTreeProducer.MCTruth.isData = cms.bool(isData)
if isData:
    process.myMiniTreeProducer.MCTruth.sampleCode = cms.string("DATA")
else:
    #for multi CRAB
    process.myMiniTreeProducer.MCTruth.sampleCode = cms.string("sampCode_")
process.myMiniTreeProducer.minEventQualityToStore = cms.int32(1)
process.myMiniTreeProducer.Trigger.source = cms.InputTag('TriggerResults::'+trigMenu)
process.myMiniTreeProducer.Trigger.bits = cms.vstring("HLT_IsoMu24", "HLT_IsoTkMu24","HLT_Mu17_TrkIsoVVL", "HLT_IsoMu22","HLT_IsoTkMu22", "HLT_DoubleEle")

#https://github.com/cms-jet/JRDatabase/tree/master/textFiles/Spring16_25nsV10_MC
process.myMiniTreeProducer.Jets.resolutionsFile = cms.string('Spring16_25nsV10_MC_PtResolution_AK8PF.txt')
process.myMiniTreeProducer.Jets.scaleFactorsFile = cms.string('Spring16_25nsV10_MC_SF_AK8PF.txt')

#-----------------------------
#ANALYSIS SEQUENCE 
#-----------------------------
process.p  = cms.Path(process.allEventsFilter*process.myMiniTreeProducer)
process.schedule = cms.Schedule(process.p)

