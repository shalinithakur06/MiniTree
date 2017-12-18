#include "MiniTree/Selection/interface/MyEventSelection.h"
#include "TrackingTools/IPTools/interface/IPTools.h"
#include "PhysicsTools/PatUtils/interface/TriggerHelper.h"
#include "DataFormats/PatCandidates/interface/TriggerEvent.h"

#include "DataFormats/MuonReco/interface/MuonSelectors.h"
std::vector<MyMuon> MyEventSelection::getMuons(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  using namespace edm;
  using namespace std;
  
  std::vector<MyMuon> selMuons; 
  selMuons.clear();
  
  try{
    //passKin
    double maxEta = configParamsMuons_.getParameter<double>("maxEta");
    double minPt = configParamsMuons_.getParameter<double>("minPt");
    //passIso
    double maxRelIso = configParamsMuons_.getParameter<double>("maxRelIso");
    TString rawtag="Muons";
    //std::string tag(rawtag);
    TString tag(rawtag);
    edm::Handle<pat::MuonCollection>imuons;
    iEvent.getByToken( Muonsources, imuons); // 76x
    if(imuons.isValid()){
      for(size_t iMuon = 0; iMuon < imuons->size(); iMuon++){
	  const pat::Muon mIt = ((*imuons)[iMuon]);
	  MyMuon newMuon = MyMuonConverter(mIt, rawtag);
	  newMuon.muName = tag;
	  //make selections
	  bool passKin = true, passId = true, passIso = true;
	  if(mIt.pt() < minPt || fabs(mIt.eta()) > maxEta) passKin = false;
      //id
	  bool isGlobal=false;
	  if(mIt.isGlobalMuon() && mIt.isTrackerMuon())isGlobal=true;
	  if(!isGlobal)passId = false;
      //iso
	  if(newMuon.pfRelIso > maxRelIso)passIso = false;
      int quality = 0;
	  if(passKin)quality  = 1; // quality = 0000 0000 0000 0001 = 1
	  if(passId)quality |= 1<<1;// quality =  0000 0000 0000 0001 | 0000 0000 0000 0010 = 3
	  if(passIso)quality |= 1<<2;// quality = 0000 0000 0000 0011 | 0000 0000 0000 0100 = 7
	  newMuon.quality = quality;
	  //if(passKin && passId && passIso) selMuons.push_back(newMuon);
	  if(passKin) selMuons.push_back(newMuon);
	  }
    }
  }catch(std::exception &e){
    std::cout << "[Muon Selection] : check selection " << e.what() << std::endl;
  }
  return selMuons;
}
  

MyMuon MyEventSelection::MyMuonConverter(const pat::Muon& iMuon, TString& dirtag)
{
  MyMuon newMuon;
  newMuon.Reset();
 
  ///basic 
  newMuon.charge = iMuon.charge();
  const reco::GenParticle *gen = iMuon.genLepton();
  if(gen){
    newMuon.gen_id = gen->pdgId();
    if(gen->numberOfMothers() > 0)
      newMuon.gen_mother_id = gen->mother()->pdgId();
  }
  newMuon.p4.SetCoordinates(iMuon.px(), iMuon.py(), iMuon.pz(), iMuon.p());
  newMuon.type = iMuon.type();
  newMuon.vertex.SetCoordinates(iMuon.vx(), iMuon.vy(), iMuon.vz()); 
  myhistos_["pt_"+dirtag]->Fill(iMuon.pt());
  myhistos_["eta_"+dirtag]->Fill(iMuon.eta());
  myhistos_["phi_"+dirtag]->Fill(iMuon.phi());
  
  ///id
  //Loose
  newMuon.isGlobalMuon = iMuon.isGlobalMuon();
  newMuon.isPFMuon = iMuon.isPFMuon();
  newMuon.isTrackerMuon = iMuon.isTrackerMuon();
  //Medium
  if(iMuon.isGlobalMuon()){
    newMuon.chi2LocalPosition = iMuon.combinedQuality().chi2LocalPosition;
    //global track
    const reco::TrackRef gmTrack = iMuon.globalTrack();
    if(!gmTrack.isNull()){
      newMuon.normChi2 = gmTrack->normalizedChi2();
      newMuon.nMuonHits = gmTrack->hitPattern().numberOfValidMuonHits();
      myhistos_["normChi2_"+dirtag]->Fill(gmTrack->normalizedChi2());
      myhistos_["nHits_"+dirtag]->Fill(gmTrack->numberOfValidHits());
      myhistos_["nMuonHits_"+dirtag]->Fill(newMuon.nMuonHits);
      }
    //best track
    const reco::TrackRef bmTrack = iMuon.muonBestTrack();
    if(!bmTrack.isNull()){
      newMuon.D0 = fabs(bmTrack->dxy(refVertex_.position())); 
      newMuon.Dz = fabs(bmTrack->dz(refVertex_.position()));
      myhistos_["D0_"+dirtag]->Fill(newMuon.D0);
      myhistos_["Dz_"+dirtag]->Fill(newMuon.Dz);
      }
    //inner track
    const reco::TrackRef imTrack = iMuon.innerTrack();
    if(!imTrack.isNull()){
      newMuon.validFraction = imTrack->validFraction();
      newMuon.nPixelHits = imTrack->hitPattern().numberOfValidPixelHits(); 
      newMuon.nTrackerLayers = imTrack->hitPattern().trackerLayersWithMeasurement(); 
      }
  }
  
  newMuon.segmentCompatibility = muon::segmentCompatibility(iMuon);
  newMuon.trkKink = iMuon.combinedQuality().trkKink;
  //Tight
  newMuon.nMatchedStations = iMuon.numberOfMatchedStations();
  
  ///iso
  std::vector<double> pfiso = defaultPFMuonIsolation(iMuon); 
  newMuon.ChHadIso = pfiso[0]; 
  newMuon.PhotonIso = pfiso[1]; 
  newMuon.NeuHadIso = pfiso[2]; 
  newMuon.PileupIso = pfiso[3];
  newMuon.pfRelIso = pfiso[4]; 
  myhistos_["pfRelIso_"+dirtag]->Fill(pfiso[4]); 
  return newMuon;
}

std::vector<double> MyEventSelection::defaultPFMuonIsolation (const pat::Muon& muon) 
{ 
  std::vector<double> values(5,0); 
  values[0] = muon.pfIsolationR04().sumChargedHadronPt; 
  values[1] = muon.pfIsolationR04().sumPhotonEt; 
  values[2] = muon.pfIsolationR04().sumNeutralHadronEt;
  values[3] = muon.pfIsolationR04().sumPUPt;
  values[4] = (muon.pfIsolationR04().sumChargedHadronPt + std::max(0., muon.pfIsolationR04().sumNeutralHadronEt+muon.pfIsolationR04().sumPhotonEt-0.5*muon.pfIsolationR04().sumPUPt))/muon.pt();

  return values;
}
