#include "MiniTree/Selection/interface/MyEventSelection.h"

std::vector<MyMET> MyEventSelection::getMETs(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  using namespace edm;
  
  std::vector<MyMET> selMETs; 
  selMETs.clear();

  try{
    //config parameters
	TString rawtag="METs";
	std::string tag(rawtag);
	
	edm::Handle<pat::METCollection>imets;
	iEvent.getByToken( Metsources, imets);
    if(imets.isValid()){ 
    const pat::MET metIt = ((*imets)[0]);
	MyMET newMET = MyMETConverter(metIt, rawtag);
	newMET.metName = tag;
	selMETs.push_back(newMET);
    }
  }catch(std::exception &e){
    std::cout << "[MET Selection] : check selection " << e.what() << std::endl;
  }
  
  return selMETs;
}

  
MyMET MyEventSelection::MyMETConverter(const pat::MET& iMET, TString& dirtag)
{
  MyMET newMET;
  newMET.Reset();
  
  //pfMET
  if(iMET.isPFMET()){
    newMET.emEtFraction = iMET.NeutralEMFraction()+ iMET.ChargedEMEtFraction();
    newMET.hadEtFraction = iMET.NeutralHadEtFraction()+ iMET.ChargedHadEtFraction();
    newMET.isPFMET = true;
    newMET.muonEtFraction = iMET.MuonEtFraction();
  }
  newMET.metSignificance = iMET.significance();
  newMET.sumEt = iMET.sumEt();
  newMET.p4.SetCoordinates(iMET.px(), iMET.py(), 0, iMET.et());
  myhistos_["pt_"+dirtag]->Fill(iMET.pt());
  myhistos_["sumet_"+dirtag]->Fill(iMET.sumEt());
  myhistos_["phi_"+dirtag]->Fill(iMET.phi());
  return newMET;
}
