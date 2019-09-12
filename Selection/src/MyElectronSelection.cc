#include "MiniTree/Selection/interface/MyEventSelection.h"
#include "TrackingTools/IPTools/interface/IPTools.h"
#include "PhysicsTools/PatUtils/interface/TriggerHelper.h"
#include "DataFormats/PatCandidates/interface/TriggerEvent.h"
#include "RecoEgamma/EgammaTools/interface/ConversionTools.h"
#include "EgammaAnalysis/ElectronTools/interface/ElectronEffectiveArea.h"

std::vector<MyElectron> MyEventSelection::getElectrons(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  using namespace edm;
  using namespace std; 
  std::vector<MyElectron> selElectrons; 
  selElectrons.clear();
  
  try{
    //std::string id = configParamsElectrons_.getParameter<std::string>("id");
    double maxRelCombPFIsoEA = configParamsElectrons_.getParameter<double>("maxRelCombPFIsoEA");
    double minEt = configParamsElectrons_.getParameter<double>("minEt");
    double maxEta = configParamsElectrons_.getParameter<double>("maxEta");
    std::string triggerMatch = configParamsElectrons_.getParameter<std::string>("triggerMatch");  
    
    //Compute the rho, for relCombPFIsoEAcorr
    edm::Handle<double> hRho;
    iEvent.getByToken(eventrhoToken_, hRho);
    double rho_ = hRho.isValid() ? *hRho : 0;
   
    //Get the conversion collection
    edm::Handle<reco::ConversionCollection> conversions;
    iEvent.getByToken(conversionsMiniAODToken_, conversions);
    
    //Get the beam spot
    edm::Handle<reco::BeamSpot> theBeamSpot;
    iEvent.getByToken(beamSpotToken_,theBeamSpot);
    
    //get electrons
    TString rawtag="Electrons";
    TString tag(rawtag);//std::string tag(rawtag);
    
    edm::Handle<pat::ElectronCollection>ieles;
    iEvent.getByToken(Elesources, ieles); 
    
    if(ieles.isValid()){
      for(size_t iEle = 0; iEle < ieles->size(); iEle++){
	    const pat::Electron eIt = ((*ieles)[iEle]);
	    MyElectron newElectron = MyElectronConverter(eIt, rawtag);
        newElectron.eleName = tag; ///Memory leak with std::string tag(rawtag)
        
        //Make selections
        bool passKin = true, passIso = true;
	    if(newElectron.p4.Et() < minEt || 
	       fabs(newElectron.p4.Eta()) > maxEta) passKin = false;
        
        //Rel comb PF iso with EA
	newElectron.eleRho = rho_;
	    newElectron.relCombPFIsoEA = relCombPFIsoWithEAcorr(eIt, rho_, rawtag); 
        if(newElectron.relCombPFIsoEA > maxRelCombPFIsoEA) passIso = false;
        
        int quality = 0;
	    if(passKin)quality  = 1;
        if(passIso)quality |= 1<<2;
	    newElectron.quality = quality;
	    if(passKin)selElectrons.push_back(newElectron);
      }//for loop
    }
  }catch(std::exception &e){
    std::cout << "[Electron Selection] : check selection " << e.what() << std::endl;
  }  
  return selElectrons;
}

    
MyElectron MyEventSelection::MyElectronConverter(const pat::Electron& iEle, TString& dirtag)
{
  MyElectron newElectron;
  newElectron.Reset();
  ///basic
  newElectron.charge = iEle.charge(); 
  const reco::GenParticle *gen = iEle.genLepton();
  if(gen){
    newElectron.gen_id = gen->pdgId();
    if(gen->numberOfMothers() > 0)
      newElectron.gen_mother_id = gen->mother()->pdgId();
  }
  reco::SuperClusterRef sc = iEle.superCluster();
  double electronSCEta = sc->eta();
  newElectron.p4.SetCoordinates(iEle.px(), iEle.py(), iEle.pz(), iEle.energy());
  //std::cout << "iEle.energy() =" << iEle.energy()<< endl;
  newElectron.eleSCEta = electronSCEta;
  newElectron.vertex.SetCoordinates(iEle.vx(), iEle.vy(), iEle.vz());
  
  myhistos_["pt_"+dirtag]->Fill(iEle.pt());
  myhistos_["eta_"+dirtag]->Fill(iEle.eta());
  myhistos_["phi_"+dirtag]->Fill(iEle.phi());
  
  ///sel 
  newElectron.sigmaIetaIeta = iEle.full5x5_sigmaIetaIeta();
  newElectron.energy5x5 = iEle.e5x5();
  newElectron.energy2x5 = iEle.e2x5Max();
  newElectron.isEcalDriven = iEle.ecalDrivenSeed();
  newElectron.GsfEleEmHadD1IsoRhoCut = iEle.dr03EcalRecHitSumEt() + iEle.dr03HcalDepth1TowerSumEt();


  //abs(dEtaInSeed)
  newElectron.dEtaInSeed = iEle.superCluster().isNonnull() && iEle.superCluster()->seed().isNonnull() ?iEle.deltaEtaSuperClusterTrackAtVtx() - iEle.superCluster()->eta() + iEle.superCluster()->seed()->eta() : std::numeric_limits<float>::max();
  //abs(dPhiIn)
  newElectron.dPhiIn = iEle.deltaPhiSuperClusterTrackAtVtx();
  newElectron.hadOverEm = iEle.hadronicOverEm();
  //abs(1/E-1/p) 
  const float ecal_energy_inverse = 1.0/iEle.ecalEnergy();
  //std::cout << "iEle.ecalEnergy()=" << iEle.ecalEnergy() << endl;
  const float eSCoverP = iEle.eSuperClusterOverP();
  newElectron.iEminusiP = std::abs(1.0 - eSCoverP)*ecal_energy_inverse;
  //expected missing inner hits
  constexpr reco::HitPattern::HitCategory missingHitType = reco::HitPattern::MISSING_INNER_HITS;
  newElectron.nInnerHits = iEle.gsfTrack()->hitPattern().numberOfAllHits(missingHitType); 
  newElectron.nInnerLostHits = iEle.gsfTrack()->hitPattern().numberOfLostTrackerHits(missingHitType); 
  //pass conversion veto
  newElectron.isPassConVeto = iEle.passConversionVeto();

  ///ids
  newElectron.isEE = iEle.isEB();
  newElectron.isEB = iEle.isEE();
  std::map<std::string, float> eidWPs; eidWPs.clear();
  const std::vector<pat::Electron::IdPair> & eids = iEle.electronIDs();
  int iid_cic = 0, iid_vbtf=0;
  for(size_t id = 0; id < eids.size(); id++){
    std::string id_name = eids[id].first;
    double id_value = eids[id].second;
    eidWPs[id_name] = id_value;
    if(id_name.find("MC") != std::string::npos){
      iid_cic++;
      if(int(id_value) & 0x1){
        myhistos_["cic_id_"+dirtag]->Fill(iid_cic);
      }
      //myhistos_["cic_id_"+dirtag]->GetXaxis()->SetBinLabel(iid_cic+1, id_name.c_str());
    }
    else{
      iid_vbtf++;
      if(int(id_value) & 0x1){
        myhistos_["vbtf_id_"+dirtag]->Fill(iid_vbtf);
      }
      //myhistos_["vbtf_id_"+dirtag]->GetXaxis()->SetBinLabel(iid_vbtf+1, id_name.c_str());
   }
  }
  newElectron.eidWPs = eidWPs;

  ///iso
  std::vector<double> pfiso = defaultPFElectronIsolation(iEle);
  newElectron.ChHadIso = pfiso[0]; 
  newElectron.PhotonIso = pfiso[1];  
  newElectron.NeuHadIso = pfiso[2];  
  newElectron.PileupIso = pfiso[3];
  newElectron.D0 = iEle.gsfTrack()->dxy(refVertex_.position());
  newElectron.Dz = iEle.gsfTrack()->dz(refVertex_.position());
  //std::cout<<"---------------------"<<std::endl;
  //std::cout<<"newElectron.D0 = "<<newElectron.D0<<endl;
  //std::cout<<"newElectron.Dz = "<<newElectron.Dz<<endl;

  newElectron.eleTrkPt = iEle.dr03TkSumPt();
  return newElectron;
}

std::vector<double> MyEventSelection::defaultPFElectronIsolation (const pat::Electron& ele)
{
  double ePt((double)ele.pt());
  double norm=std::max((double)20.0,(double)ePt);
  double puOffsetCorrection = 0.0;
  std::vector<double> values(4,0);
    values[0] = ele.chargedHadronIso();
    values[1] = ele.photonIso();
    values[2] = ele.neutralHadronIso();
    values[3] =(std::max(ele.photonIso()+ele.neutralHadronIso() - puOffsetCorrection, 0.0) + ele.chargedHadronIso())/norm;
    return values;
}

//Rel. comb. PF iso with EA corr
float MyEventSelection::relCombPFIsoWithEAcorr(const pat::Electron& iEle, double rho_, TString& dirtag)
{
  double EffArea_ = ElectronEffectiveArea::GetElectronEffectiveArea(ElectronEffectiveArea::kEleGammaAndNeutralHadronIso04, iEle.superCluster()->eta(), ElectronEffectiveArea::kEleEAData2012);
  const reco::GsfElectron::PflowIsolationVariables& pfIso = iEle.pfIsolationVariables();
  const float chad = pfIso.sumChargedHadronPt;
  const float nhad = pfIso.sumNeutralHadronEt;
  const float pho = pfIso.sumPhotonEt;
  const float relCombPFIsoEAcorr = (chad + std::max(0.0, nhad + pho - rho_* EffArea_))/iEle.pt();
  myhistos_["relCombPFIsoEA_"+dirtag]->Fill(relCombPFIsoEAcorr); 
  return relCombPFIsoEAcorr;
}
