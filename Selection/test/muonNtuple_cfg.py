import FWCore.ParameterSet.Config as cms
from MiniTree.Selection.LocalRunSkeleton_cff import *
from MiniTree.Selection.LocalSources_cff import toPrint

#-----------------------------
#INPUT & OUTPUT
#-----------------------------
isData=False
#Data
#inFile = "/store/data/Run2016B/SingleMuon/MINIAOD/23Sep2016-v3/00000/00AE0629-1F98-E611-921A-008CFA1112CC.root"
#MC
inFile = "/store/mc/RunIISummer16MiniAODv2/TT_TuneCUETP8M2T4_13TeV-powheg-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/50000/0693E0E7-97BE-E611-B32F-0CC47A78A3D8.root"
#inFile = ["file:FEDED4C8-573B-E611-9ED6-0025904CF102.root"]

process.source.fileNames = [inFile]
process.maxEvents.input = cms.untracked.int32(200)
#for multi CRAB
process.TFileService.fileName = cms.string("outFile_.root")

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
    #for multi CRAB
    process.myMiniTreeProducer.MCTruth.sampleCode = cms.string("sampCode_")
process.myMiniTreeProducer.MCTruth.producePDFweights = cms.bool(producePDFweights)
process.myMiniTreeProducer.minEventQualityToStore = cms.int32(1)
process.myMiniTreeProducer.Trigger.source = cms.InputTag('TriggerResults::'+trigMenu)
process.myMiniTreeProducer.Trigger.myTrig = "HLT_IsoMu2"
process.myMiniTreeProducer.MCTruth.sampleChannel = cms.string('muon')
process.myMiniTreeProducer.Jets.resolutionsFile = cms.string('Spring16_25nsV10_MC_PtResolution_AK4PF.txt')
process.myMiniTreeProducer.Jets.scaleFactorsFile = cms.string('Spring16_25nsV10_MC_SF_AK4PF.txt')

#-----------------------------
#ANALYSIS SEQUENCE 
#-----------------------------
process.p  = cms.Path(process.allEventsFilter*process.basePreSel*process.myMiniTreeProducer)
process.schedule = cms.Schedule(process.p)
checkProcessSchedule(storeOutPath,True)

