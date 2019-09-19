import FWCore.ParameterSet.Config as cms

from MiniTree.Selection.ObjectSelectors_cfi import *
allEventsFilter = cms.EDFilter("AllEventsFilter")

# base configuration ----------------------------------------
myMiniTreeProducer = cms.EDAnalyzer("MiniTreeProducer",
                                    MCTruth = BaseMCTruthSet,
                                    Trigger = BaseTriggerSet,
                                    Vertex = BaseVertexSet,
                                    Muons = BaseMuonsSet,
                                    Electrons = BaseElectronsSet,
                                    Jets = BaseJetsSet,
                                    Mets = BaseMetsSet,
                                    minEventQualityToStore = cms.int32(0)
                                    )
