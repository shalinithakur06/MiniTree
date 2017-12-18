#ifndef LLRAnalysis_TauTauStudies_ElectronsUserEmbedded_h
#define LLRAnalysis_TauTauStudies_ElectronsUserEmbedded_h

#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/ParameterSet/interface/FileInPath.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/PatCandidates/interface/Electron.h"

#include "DataFormats/RecoCandidate/interface/IsoDepositVetos.h"
#include "DataFormats/RecoCandidate/interface/IsoDeposit.h"
#include "DataFormats/PatCandidates/interface/Isolation.h"

//#include "EgammaAnalysis/ElectronTools/interface/EGammaMvaEleEstimator.h"
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"
#include "MagneticField/Engine/interface/MagneticField.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/TransientTrack/plugins/TransientTrackBuilderESProducer.h"
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrack.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectronFwd.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/Scalers/interface/DcsStatus.h"
#include "RecoEgamma/EgammaTools/interface/ConversionFinder.h"
#include "DataFormats/EgammaCandidates/interface/Conversion.h"
#include "DataFormats/GeometryVector/interface/VectorUtil.h"

#include "DataFormats/BeamSpot/interface/BeamSpot.h"

#include "RecoEgamma/EgammaTools/interface/ConversionTools.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHit.h"
#include "RecoEcal/EgammaCoreTools/interface/EcalClusterLazyTools.h"
#include "EgammaAnalysis/ElectronTools/interface/ElectronEffectiveArea.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"

using namespace edm;
using namespace std;
using namespace reco;

class ElectronsUserEmbedded : public edm::EDProducer{

public: 
  explicit ElectronsUserEmbedded(const edm::ParameterSet&);
  virtual ~ElectronsUserEmbedded() {};

private:
  virtual void produce(edm::Event & iEvent, const edm::EventSetup & iSetup);
  //declare token
  edm::EDGetTokenT<pat::ElectronCollection> eleToken_;
  //edm::EDGetToken electronsMiniAODToken_;
  edm::EDGetTokenT<float> rhoToken_;
  edm::EDGetTokenT<reco::VertexCollection> vertexToken_;
  edm::EDGetTokenT<reco::ConversionCollection> convToken_;
  edm::EDGetTokenT<reco::BeamSpot> beamSpotToken_;
  //ID decisions objects
  //https://github.com/ikrav/EgammaWork/blob/ntupler_and_VID_demos_8.0.3/ElectronNtupler/plugins/ElectronNtuplerVIDDemo.cc
  //edm::EDGetTokenT<edm::ValueMap<bool> > eleIdMapToken_;
  //bool isMC_;
};

#endif

ElectronsUserEmbedded::ElectronsUserEmbedded(const edm::ParameterSet & iConfig){
  edm::ConsumesCollector&& cc = consumesCollector();
  //define token
  eleToken_ = cc.consumes<pat::ElectronCollection>(iConfig.getParameter<edm::InputTag>("electronTag"));
  //electronsMiniAODToken_ = cc.consumes<edm::View<reco::GsfElectron>>(iConfig.getParameter<edm::InputTag>("electronTag"));
  vertexToken_   = cc.consumes<reco::VertexCollection>(iConfig.getParameter<edm::InputTag>("vertexTag"));
  rhoToken_ = cc.consumes<float>(iConfig.getParameter<edm::InputTag>("rho"));
  beamSpotToken_ = cc.consumes<reco::BeamSpot>(iConfig.getParameter<edm::InputTag>("beamSpot"));
  convToken_ = cc.consumes<reco::ConversionCollection>(iConfig.getParameter<edm::InputTag>("conversionsMiniAOD"));
  //eleIdMapToken_ = cc.consumes<edm::ValueMap<bool> >(iConfig.getParameter<edm::InputTag>("eleIdMap"));
  produces<pat::ElectronCollection>("");
}

void ElectronsUserEmbedded::produce(edm::Event & iEvent, const edm::EventSetup & iSetup){
  //------------------------------------
  //get various objects 
  //------------------------------------
  //get electrons
  edm::Handle<pat::ElectronCollection>electronsHandle;
  iEvent.getByToken(eleToken_, electronsHandle); 
  const pat::ElectronCollection* electrons = electronsHandle.product();
  //edm::Handle<edm::View<reco::GsfElectron> > electrons;
  //iEvent.getByToken(electronsMiniAODToken_, electrons); 
  //const reco::GsfElectron* electrons = electronsHandle.product();
  
  //get vertices
  edm::Handle<reco::VertexCollection> vertexHandle;
  iEvent.getByToken(vertexToken_,vertexHandle);
  const reco::VertexCollection* vertexes = vertexHandle.product();
  
  //Compute the rho, for relCombPFIsoEAcorr
  edm::Handle<float> hRho;
  iEvent.getByToken(rhoToken_, hRho);
  float rho_ = hRho.isValid() ? *hRho : 0;
  
  //Get the conversion collection
  edm::Handle<reco::ConversionCollection> hConversions;
  iEvent.getByToken(convToken_, hConversions);
  
  //Get the beam spot
  edm::Handle<reco::BeamSpot> bsHandle;
  iEvent.getByToken(beamSpotToken_,bsHandle);
  const reco::BeamSpot &thebs = *bsHandle.product();
  
  //get cutBased id
  //edm::Handle<edm::ValueMap<bool> > ele_id_decisions;
  //iEvent.getByToken(eleIdMapToken_ ,ele_id_decisions);

  //------------------------------------
  //loop over electrons
  //------------------------------------
  std::auto_ptr< pat::ElectronCollection > electronsUserEmbeddedColl( new pat::ElectronCollection() ) ;
  //std::auto_ptr<reco::GsfElectron> electronsUserEmbeddedColl( new reco::GsfElectron) ;
  for(size_t iEle = 0; iEle < electrons->size(); iEle++){
    pat::Electron ele( (*electrons)[iEle] );
    //for (size_t i = 0; i < electrons->size(); ++i){
    //const auto ele = electrons->ptrAt(i);
    //int isPassEleId  = int((*ele_id_decisions)[ele]);
    //ele->addUserFloat("cutBasedId_M",isPassEleId);

    //------------------------------------
    //get cut based ID manually
    ////https://twiki.cern.ch/twiki/bin/viewauth/CMS/CutBasedElectronIdentificationRun2
    //------------------------------------
    reco::SuperClusterRef sc = ele.superCluster();
    //electron super cluster eta, etc
    float eleSCEta = sc->eta();
    float sigmaIetaIeta = ele.full5x5_sigmaIetaIeta();
    float dEtaInSeed = ele.superCluster().isNonnull() && ele.superCluster()->seed().isNonnull() ?ele.deltaEtaSuperClusterTrackAtVtx() - ele.superCluster()->eta() + ele.superCluster()->seed()->eta() : std::numeric_limits<float>::max();
    float dPhiIn = ele.deltaPhiSuperClusterTrackAtVtx();
    float hadOverEm = ele.hadronicOverEm();
  
    //abs(1/E-1/p) 
    const float ecal_energy_inverse = 1.0/ele.ecalEnergy();
    const float eSCoverP = ele.eSuperClusterOverP();
    float iEminusiP = std::abs(1.0 - eSCoverP)*ecal_energy_inverse;
    
    //expected missing inner hits
    constexpr reco::HitPattern::HitCategory missingHitType = reco::HitPattern::MISSING_INNER_HITS;
    int nInnerHits = ele.gsfTrack()->hitPattern().numberOfHits(missingHitType);
    
    //pass conversion veto
    bool isPassConVeto = !ConversionTools::hasMatchedConversion(ele, hConversions, thebs.position());
    
    //Rel. comb. PF iso with EA corr   
    float EffArea_ = ElectronEffectiveArea::GetElectronEffectiveArea(ElectronEffectiveArea::kEleGammaAndNeutralHadronIso04, ele.superCluster()->eta(), ElectronEffectiveArea::kEleEAData2012);
    const reco::GsfElectron::PflowIsolationVariables& pfIso = ele.pfIsolationVariables();
    const float chad = pfIso.sumChargedHadronPt;
    const float nhad = pfIso.sumNeutralHadronEt;
    const float pho = pfIso.sumPhotonEt;
    float default_iso = 0.0;
    const float relCombPFIsoEAcorr = (chad + std::max(default_iso, nhad + pho - rho_* EffArea_))/ele.pt();
    ele.addUserFloat("PFRelIso04", relCombPFIsoEAcorr);
    
    //distance w.r.t primary vertex
    float dxyWrtPV =  -99.;
    float dzWrtPV  =  -99.;
    if(vertexes->size()!=0 && (ele.gsfTrack()).isNonnull() ){
      dxyWrtPV = (ele.gsfTrack())->dxy( (*vertexes)[0].position() ) ;
      dzWrtPV  = (ele.gsfTrack())->dz(  (*vertexes)[0].position() ) ;
    }
    else if (vertexes->size()!=0 && (ele.track()).isNonnull() ){
      dxyWrtPV = (ele.track())->dxy( (*vertexes)[0].position() ) ;
      dzWrtPV  = (ele.track())->dz(  (*vertexes)[0].position() ) ;
    }
    ele.addUserFloat("dxyWrtPV",dxyWrtPV);
    ele.addUserFloat("dzWrtPV",dzWrtPV);
    
    //get medium ID
    bool passID = false;
    //barrel
    if(abs(eleSCEta) <= 1.479
       && sigmaIetaIeta        < 0.00998
       && abs(dEtaInSeed)      < 0.00311
       && abs(dPhiIn)          < 0.103
       && hadOverEm            < 0.253
       ///&& ele.relCombPFIsoEA    < 0.0695        
       && abs(iEminusiP)       < 0.134
       && nInnerHits           <= 1
       && isPassConVeto
      )passID = true;

    //endcap
    if(abs(eleSCEta) > 1.479
       && sigmaIetaIeta        < 0.0298
       && abs(dEtaInSeed)      < 0.00609
       && abs(dPhiIn)          < 0.045
       && hadOverEm            < 0.0878
       ///&& ele.relCombPFIsoEA    < 0.0821        
       && abs(iEminusiP)       < 0.13
       && nInnerHits           <= 1
       && isPassConVeto
       )passID = true;
    ele.addUserInt("cutBasedID", int(passID));
    /*
    if(int(passID)>0){
    cout<<"--------------------------"<<endl;
    cout<<"sigmaIetaIeta = "<<sigmaIetaIeta<<endl;
    cout<<"dEtaInSeed = "<<dEtaInSeed<<endl;
    cout<<"dPhiIn = "<<dPhiIn<<endl;
    cout<<"hadOverEm = "<<hadOverEm<<endl;
    cout<<"iEminusiP = "<<iEminusiP<<endl;
    cout<<"nInnerHits = "<<nInnerHits<<endl;
    cout<<"conVeto = "<<isPassConVeto<<endl;
    cout<<"relIso = "<<relCombPFIsoEAcorr<<endl;
    cout<<"cutBasedID = "<<int(passID)<<endl;
    }
    */
    //push selected electrons
    electronsUserEmbeddedColl->push_back(ele);
  }    
  //put selected electrons back in to the event
  iEvent.put( electronsUserEmbeddedColl );
  return;
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(ElectronsUserEmbedded);
