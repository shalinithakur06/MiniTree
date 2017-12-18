import FWCore.ParameterSet.Config as cms

#my base values for trigger bit selection -------------------
#BaseTriggerSet = cms.PSet( source = cms.InputTag("TriggerResults::HLT"),
BaseTriggerSet = cms.PSet( source = cms.InputTag("TriggerResults::HLT2"),
                           bits = cms.vstring('HLT_IsoMu24','HLT_Ele27_WPTight_Gsf','HLT_JetE30_NoBPTX'),
                           myTrig = cms.string('HLT_IsoMu24')
                           )

#my base values for vertex selection ---------------------------------
BaseVertexSet = cms.PSet( vertexSource = cms.InputTag("offlineSlimmedPrimaryVertices"),
                          maxZ = cms.double(24),
                          rho =  cms.InputTag('fixedGridRhoAll'),
                          maxRho = cms.double(2.0),
                          minNDOF = cms.int32(4),
                          beamSpotSource = cms.InputTag("offlineBeamSpot"),
                          useBeamSpot = cms.bool(True)
                          )

#my base values for tracks selection ----------------------------------
BaseTrackSet = cms.PSet( source = cms.InputTag("generalTracks"),
                         dedxSource = cms.InputTag("dedxHarmonic2"),
                         minPt = cms.double(1),
                         maxEta = cms.double(2.5),
                         minQuality = cms.int32(2),
                         maxD0 = cms.double(400),
                         maxTrackChi2 = cms.double(10),
                         minTrackValidHits = cms.int32(5),
                         minPixelHits = cms.int32(1),
                         minPrimaryVertexWeight = cms.double(0),
                         minDeltaRtoLepton = cms.double(0.1),
                         trackIsoCone = cms.double(0.3),
                         maxPtSumInTrackIsoCone = cms.double(10)
                         )


#my base values for muon selection ---------------------------------------
BaseMuonsSet =  cms.PSet( sources = cms.InputTag("slimmedMuons"),
                          #triggerEvent = cms.InputTag("patTriggerEvent"),
                          triggerEvent = cms.InputTag("patTrigger"),
                          triggerMatch = cms.string("TrigMatch"),
                          minPt = cms.double(10),
                          maxEta = cms.double(2.4),
                          maxRelIso = cms.double(0.50),
                          )

# base values for electron selection ----------------------------------------------
BaseElectronsSet =  cms.PSet(sources = cms.InputTag("slimmedElectrons"),
                        dedxSource = cms.InputTag("dedxHarmonic2"),
                        #triggerEvent = cms.InputTag("patTriggerEvent"),
                        triggerEvent = cms.InputTag("patTrigger"),
                        triggerMatch = cms.string("TrigMatch"),
                        maxRelCombPFIsoEA = cms.double(0.4),
                        minEt = cms.double(10),
                        maxEta = cms.double(2.5),
                        mvacut = cms.double(0.30),
                        #rho = cms.InputTag("kt6PFJets", "rho")
#https://github.com/ikrav/cmssw/blob/egm_id_80X_v1/RecoEgamma/ElectronIdentification/python/Identification/cutBasedElectronID_tools.py#L317
                        rhoIso = cms.InputTag("fixedGridRhoFastjetAll"),
                        #https://github.com/ikrav/EgammaWork/blob/ntupler_and_VID_demos_8.0.3/ElectronNtupler/test/runElectrons_VID_CutBased_Summer16_HLTSafe_demo.py
                        conversionsMiniAOD  = cms.InputTag('reducedEgamma:reducedConversions'),
                        beamSpot = cms.InputTag('offlineBeamSpot'),
                        eleIdVerbose = cms.bool(False)
                        )

#my base values for jet selection -----------------------------------------------
BaseJetsSet = cms.PSet(sources = cms.InputTag("slimmedJets"),
                       CaloJetId = cms.PSet( version = cms.string("PURE09"), quality = cms.string("LOOSE") ),
                       PFJetId = cms.PSet( version = cms.string("FIRSTDATA"), quality = cms.string("LOOSE") ),
                       dedxSource = cms.InputTag("dedxHarmonic2"),
                       triggerEvent = cms.InputTag("hltTriggerSummaryAOD"),
                       #triggerEvent = cms.InputTag("patTriggerEvent"),
                       #triggerEvent = cms.InputTag("selectedPatTrigger"),
                       triggerMatch = cms.string("TrigMatch"),
                       useRawJets = cms.bool(False),
                       minPt = cms.double(17),
                       maxEta = cms.double(2.5),
                       minDeltaRtoLepton = cms.double(0.4),
                       puMVADiscriminant = cms.InputTag("puJetMva:fullDiscriminant"),
                       puMVAID = cms.InputTag("puJetMva:fullId"),
                       puMVADiscriminantResCor = cms.InputTag("puJetMvaResCor:fullDiscriminant"),
                       puMVAIDResCor = cms.InputTag("puJetMvaResCor:fullId"),
                       jet_rho = cms.InputTag('fixedGridRhoAll'),
                       #resolutionsFile = cms.FileInPath('CondFormats/JetMETObjects/data/Summer15_V0_MC_JER_AK4PFchs.txt'),
                       #scaleFactorsFile = cms.FileInPath('CondFormats/JetMETObjects/data/Summer12_V1_MC_JER_SF_AK5PFchs.txt')
                       #resolutionsFile = cms.FileInPath("MiniTree/Selection/test/Spring16_25nsV6_MC_PtResolution_AK4PF.txt"),
                       #scaleFactorsFile = cms.FileInPath("MiniTree/Selection/test/Spring16_25nsV6_MC_SF_AK4PF.txt")
                       resolutionsFile = cms.string("Spring16_25nsV10_MC_PtResolution_AK4PF.txt"),
                       scaleFactorsFile= cms.string("Spring16_25nsV10_MC_SF_AK4PF.txt")
                       )

#my base values for met selection ------------------------------------------------
BaseMetsSet = cms.PSet(sources = cms.InputTag("slimmedMETs"),
                       minMET = cms.double(10)
                       )

#my MC truth matching sets -------------------------------------------------------
BaseMCTruthSet = cms.PSet( isData = cms.bool(False),
                       producePDFweights = cms.bool(False),
                       sampleCode = cms.string("SEECODES"),
                       sampleChannel = cms.string("electron"),
                       jpMatchSources = cms.VInputTag("selectedPatJetsByRef", "selectedPatJetsAK5JPTByRef", "selectedPatJetsAK5PFByRef", "selectedPatJetsPFlowByRef")
                       )

BaseKFPSet = cms.PSet(sources = cms.VInputTag("kinFitTtSemiLepEvent:Leptons","kinFitTtSemiLepEvent:Neutrinos","kinFitTtSemiLepEvent:PartonsHadB","kinFitTtSemiLepEvent:PartonsHadP","kinFitTtSemiLepEvent:PartonsHadQ","kinFitTtSemiLepEvent:PartonsLepB","kinFitTtSemiLepEventJESUp:Leptons","kinFitTtSemiLepEventJESUp:Neutrinos","kinFitTtSemiLepEventJESUp:PartonsHadB","kinFitTtSemiLepEventJESUp:PartonsHadP","kinFitTtSemiLepEventJESUp:PartonsHadQ","kinFitTtSemiLepEventJESUp:PartonsLepB","kinFitTtSemiLepEventJESDown:Leptons","kinFitTtSemiLepEventJESDown:Neutrinos","kinFitTtSemiLepEventJESDown:PartonsHadB","kinFitTtSemiLepEventJESDown:PartonsHadP","kinFitTtSemiLepEventJESDown:PartonsHadQ","kinFitTtSemiLepEventJESDown:PartonsLepB","kinFitTtSemiLepEventJERUp:Leptons","kinFitTtSemiLepEventJERUp:Neutrinos","kinFitTtSemiLepEventJERUp:PartonsHadB","kinFitTtSemiLepEventJERUp:PartonsHadP","kinFitTtSemiLepEventJERUp:PartonsHadQ","kinFitTtSemiLepEventJERUp:PartonsLepB","kinFitTtSemiLepEventJERDown:Leptons","kinFitTtSemiLepEventJERDown:Neutrinos","kinFitTtSemiLepEventJERDown:PartonsHadB","kinFitTtSemiLepEventJERDown:PartonsHadP","kinFitTtSemiLepEventJERDown:PartonsHadQ","kinFitTtSemiLepEventJERDown:PartonsLepB"),
                      njetsUsed = cms.InputTag("kinFitTtSemiLepEvent:NumberOfConsideredJets"),
                      chi2OfFit = cms.InputTag("kinFitTtSemiLepEvent:Chi2"),
                      probOfFit = cms.InputTag("kinFitTtSemiLepEvent:Prob"),
                      statusOfFit = cms.InputTag("kinFitTtSemiLepEvent:Status"),
                      njetsUsedUp = cms.InputTag("kinFitTtSemiLepEventJESUp:NumberOfConsideredJets"),
                      chi2OfFitUp = cms.InputTag("kinFitTtSemiLepEventJESUp:Chi2"),
                      probOfFitUp = cms.InputTag("kinFitTtSemiLepEventJESUp:Prob"),
                      statusOfFitUp = cms.InputTag("kinFitTtSemiLepEventJESUp:Status"),
                      njetsUsedDown = cms.InputTag("kinFitTtSemiLepEventJESDown:NumberOfConsideredJets"),
                      chi2OfFitDown = cms.InputTag("kinFitTtSemiLepEventJESDown:Chi2"),
                      probOfFitDown = cms.InputTag("kinFitTtSemiLepEventJESDown:Prob"),
                      statusOfFitDown = cms.InputTag("kinFitTtSemiLepEventJESDown:Status"),
                      njetsUsedJERUp = cms.InputTag("kinFitTtSemiLepEventJERUp:NumberOfConsideredJets"),
                      chi2OfFitJERUp = cms.InputTag("kinFitTtSemiLepEventJERUp:Chi2"),
                      probOfFitJERUp = cms.InputTag("kinFitTtSemiLepEventJERUp:Prob"),
                      statusOfFitJERUp = cms.InputTag("kinFitTtSemiLepEventJERUp:Status"),
                      njetsUsedJERDown = cms.InputTag("kinFitTtSemiLepEventJERDown:NumberOfConsideredJets"),
                      chi2OfFitJERDown = cms.InputTag("kinFitTtSemiLepEventJERDown:Chi2"),
                      probOfFitJERDown = cms.InputTag("kinFitTtSemiLepEventJERDown:Prob"),
                      statusOfFitJERDown = cms.InputTag("kinFitTtSemiLepEventJERDown:Status"),
                      runKineFitter = cms.bool(False)
                      )
