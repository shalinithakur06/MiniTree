import FWCore.ParameterSet.Config as cms
from MiniTree.Selection.LocalSources_cff import toPrint
#
# pre selection
#
def defineBasePreSelection(process,
                           useTechTriggerBits=False,
                           filterHBHEnoise=True,
                           egmuTriggerFilter='',
                           egtriglist=['HLT_Ele10_LW_L1R'],
                           mutriggerlist=['HLT_Mu9'],
                           jettriglist=['HLT_Jet15U'],
                           trigMenu='HLT') :

    #
    # pre-selection sequences
    #

    # cut on monster events (may low quality tracks) -------------------------------
    process.load("HLTrigger.special.hltPhysicsDeclared_cfi")
    process.hltPhysicsDeclared.L1GtReadoutRecordTag = 'gtDigis'
    #process.noScraping= cms.EDFilter("FilterOutScraping",
    #                                 applyfilter = cms.untracked.bool(True),
    #                                 debugOn = cms.untracked.bool(False), ## Or 'True' to get some per-event info
    #                                 numtrack = cms.untracked.uint32(10),
    #                                 thresh = cms.untracked.double(0.25)
    #                                 )

    # good vertex filter ---------------------------------------------------------
    process.primaryVertexFilter = cms.EDFilter("GoodVertexFilter",
                                               vertexCollection = cms.InputTag('offlineSlimmedPrimaryVertices'),
                                               minimumNDOF = cms.uint32(3),
                                               maxAbsZ = cms.double(24),
                                               maxd0 = cms.double(2) )


    toPrint("Base preselection will remove PKAM and filter primary vertices","")
    #process.basePreSel = cms.Sequence(process.hltPhysicsDeclared*process.noScraping*process.primaryVertexFilter)
    process.basePreSel = cms.Sequence(process.primaryVertexFilter) # 76x

    # BSC activity ---------------------------------------------------------------
    if(useTechTriggerBits) :
        process.load('L1TriggerConfig.L1GtConfigProducers.L1GtTriggerMaskTechTrigConfig_cff')
        process.load('HLTrigger/HLTfilters/hltLevel1GTSeed_cfi')
        process.hltLevel1GTSeed.L1TechTriggerSeeding = cms.bool(True)
        # process.hltLevel1GTSeed.L1SeedsLogicalExpression = cms.string('(0 AND 9) AND (40 OR 41) AND NOT (36 OR 37 OR 38 OR 39)')
        #process.hltLevel1GTSeed.L1SeedsLogicalExpression = cms.string('0 AND NOT (36 OR 37 OR 38 OR 39)')
        process.hltLevel1GTSeed.L1SeedsLogicalExpression = cms.string('0')
        toPrint("Adding technical trigger bits", process.hltLevel1GTSeed.L1SeedsLogicalExpression)
        process.basePreSel = cms.Sequence( process.basePreSel*process.hltLevel1GTSeed )


    # HB/HE noise filter ----------------------------------------------------
    if(filterHBHEnoise) :
        process.load('CommonTools.RecoAlgos.HBHENoiseFilter_cfi')
        process.HBHENoiseFilter.minIsolatedNoiseSumE = cms.double(999999.)
        process.HBHENoiseFilter.minNumIsolatedNoiseChannels = cms.int32(999999)
        process.HBHENoiseFilter.minIsolatedNoiseSumEt = cms.double(999999.)
        toPrint("Adding HB/HE noise filter","")
        process.basePreSel = cms.Sequence( process.basePreSel*process.HBHENoiseFilter )

    # single lepton inclusive triggers
    from HLTrigger.HLTfilters.hltHighLevel_cfi import hltHighLevel as trigbit_selector
    trigbit_selector.throw = cms.bool(False)
    process.eg_selector = trigbit_selector.clone()
    process.eg_selector.TriggerResultsTag = cms.InputTag('TriggerResults', '', trigMenu)
    process.eg_selector.andOr=cms.bool(True)
    process.eg_selector.HLTPaths = cms.vstring(egtriglist)
    process.mu_selector = process.eg_selector.clone()
    process.mu_selector.andOr=cms.bool(True)
    process.mu_selector.HLTPaths = cms.vstring(mutriggerlist)
    process.egmu_selector = process.eg_selector.clone()
    process.egmu_selector.HLTPaths.extend( process.mu_selector.HLTPaths)
    process.egmu_selector.andOr=cms.bool(True)
    process.jet_selector = process.eg_selector.clone()
    process.jet_selector.andOr=cms.bool(True)
    process.jet_selector.HLTPaths = cms.vstring(jettriglist)

    #trigger sequences
    if(egmuTriggerFilter=="inclusive_eg") :
        process.inclusive_eg = cms.Sequence( process.eg_selector )
        process.basePreSel = cms.Sequence( process.inclusive_eg*process.basePreSel )
    elif(egmuTriggerFilter=="exclusive_eg") :
        process.exclusive_eg = cms.Sequence( ~process.mu_selector*process.eg_selector )
        process.basePreSel = cms.Sequence( process.exclusive_eg*process.basePreSel )
    elif(egmuTriggerFilter=="inclusive_mu") :
        process.inclusive_mu = cms.Sequence( process.mu_selector )
        process.basePreSel = cms.Sequence( process.inclusive_mu*process.basePreSel )
    elif(egmuTriggerFilter=="exclusive_mu") :
        process.exclusive_mu = cms.Sequence( ~process.eg_selector*process.mu_selector )
        process.basePreSel = cms.Sequence( process.exclusive_mu*process.basePreSel )
    elif(egmuTriggerFilter=="inclusive_egmu") :
        process.inclusive_egmu = cms.Sequence( process.egmu_selector )
        process.basePreSel = cms.Sequence( process.inclusive_egmu*process.basePreSel )
    elif(egmuTriggerFilter=="inclusive_jet") :
        process.inclusive_jet = cms.Sequence( process.jet_selector )
        process.basePreSel = cms.Sequence(process.inclusive_jet*process.basePreSel )
    if(len(egmuTriggerFilter)>0) :
        toPrint("EG/Mu trigger filter defined as", egmuTriggerFilter)

    toPrint("Data preselection sequence is",process.basePreSel)
    toPrint("Note: process.basePreSel has to be added to your main sequence","")

