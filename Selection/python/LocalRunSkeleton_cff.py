import FWCore.ParameterSet.Config as cms

procName='USERLOCAL'
mutrig = 'HLT_Mu9'
egtriglist = ['HLT_Ele15_SW_L1R']
jettrig = 'HLT_Jet30U'
trigpath = ''
runon35x = False
isData=True

# Starting with a skeleton process which gets imported with the following line
from PhysicsTools.PatAlgos.patTemplate_cfg import *
process.setName_(procName)
process.GlobalTag.globaltag = cms.string( '80X_mcRun2_asymptotic_2016_miniAODv2_v1' )


#-- Meta data to be logged in DBS ---------------------------------------------
process.configurationMetadata = cms.untracked.PSet(
    version = cms.untracked.string('$Revision: 1.3 $'),
    name = cms.untracked.string('$Source: /local/reps/CMSSW/UserCode/anayak/MiniTree/Selection/python/LocalRunSkeleton_cff.py,v $'),
    annotation = cms.untracked.string('top dileptons analysis')
    )

process.MessageLogger.cerr.FwkReport.reportEvery = 100
process.options   = cms.untracked.PSet( wantSummary = cms.untracked.bool(False) )

# data preselection -----------------------------------------------------------
from MiniTree.Selection.DataPreselSequences_cff import *

#filter to count all events processed
process.load("MiniTree.Selection.alleventsfilter_cfi")

# input/output -----------------------------------------------------------------
from MiniTree.Selection.LocalSources_cff import *
process.source.fileNames = dilcands
process.maxEvents.input = cms.untracked.int32(-1)

process.out.fileName = 'data.root'
process.out.splitLevel = cms.untracked.int32(99)
process.out.overrideInputFileSplitLevels = cms.untracked.bool(True)
process.out.outputCommands = cms.untracked.vstring('keep *')
process.outpath = cms.EndPath(process.out)

# tfileservice ------------------------------------------------------------------
process.load("PhysicsTools.UtilAlgos.TFileService_cfi")
process.TFileService.fileName = cms.string('Monitor.root')

# adds/removes the outpath
def checkProcessSchedule(storeOutPath=True, debug=True) :

    if( storeOutPath ) :
        process.schedule.append( process.outpath )
    else :
        del process.outpath
    if(debug): print process.schedule




