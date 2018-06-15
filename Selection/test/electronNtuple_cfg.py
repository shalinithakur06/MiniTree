import FWCore.ParameterSet.Config as cms
from MiniTree.Selection.LocalRunSkeleton_cff import *
from MiniTree.Selection.LocalSources_cff import toPrint

#-----------------------------------------
#INPUT & OUTPUT
#-----------------------------------------
isData=False
#Data
#inFile = "/store/data/Run2016C/SingleElectron/MINIAOD/03Feb2017-v1/100000/02169BE7-81EB-E611-BB99-02163E0137CD.root"
#MC
inFile = "/store/mc/RunIIFall17MiniAOD/QCD_Pt-120to170_MuEnrichedPt5_TuneCP5_13TeV_pythia8/MINIAODSIM/94X_mc2017_realistic_v10-v1/50000/9EB88BFC-67DE-E711-8FEF-0017A477107C.root"
#inFile = ["file:FEDED4C8-573B-E611-9ED6-0025904CF102.root"]

process.source.fileNames = [inFile]
process.maxEvents.input = cms.untracked.int32(1000)

#for multi CRAB
process.TFileService.fileName = cms.string("outFile_.root")

#-----------------------------------------
#CONFIG PARAMETERS
#-----------------------------------------
procName='LOCALUSER'
#trigMenu = 'HLT2' #https://twiki.cern.ch/twiki/bin/view/CMSPublic/WorkBookMiniAOD
trigMenu = 'HLT'
isFastsim = False
trigpath = ''

#-----------------------------------------
#START PROCESS CONFIGURATION
#-----------------------------------------
process.setName_(procName)
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff")
process.GlobalTag.globaltag  = cms.string('94X_mc2017_realistic_v10')

#-----------------------------------------
#CONFIGURE THE EXTRA MODULE
#-----------------------------------------
defineBasePreSelection(process,False, False)
storeOutPath=False
producePDFweights=False

#-----------------------------------------
#ADD THE ANALYSIS MODULE
#-----------------------------------------
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
process.myMiniTreeProducer.Trigger.myTrig = "HLT_Ele2"
process.myMiniTreeProducer.MCTruth.sampleChannel = cms.string('electron')
process.myMiniTreeProducer.Jets.resolutionsFile = cms.string('Spring16_25nsV10_MC_PtResolution_AK8PF.txt')
process.myMiniTreeProducer.Jets.scaleFactorsFile = cms.string('Spring16_25nsV10_MC_SF_AK8PF.txt')

#-----------------------------------------
#ANALYSIS SEQUENCE
#-----------------------------------------
process.p  = cms.Path(process.allEventsFilter*process.basePreSel*process.myMiniTreeProducer)
process.schedule = cms.Schedule(process.p)
checkProcessSchedule(storeOutPath,True)

