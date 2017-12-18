#include "MiniTree/Selection/interface/MyEventSelection.h"
#include "SimDataFormats/GeneratorProducts/interface/LHEEventProduct.h"

std::vector<MyMCParticle> MyEventSelection::getMCParticles(const edm::Event& iEvent){
  
  std::vector<MyMCParticle> mcParticles;
  edm::Handle<reco::GenParticleCollection> genParticles;
  try{
    iEvent.getByToken(GenParticle_, genParticles); 
  }catch(...) {;}
  
  //----------------------------------------------
  //PDG codes of the particles:http: //pdg.lbl.gov/2007/reviews/montecarlorpp.pdf
  //http://home.thep.lu.se/~torbjorn/pythia82html/ParticleProperties.html
  //----------------------------------------------

  if(genParticles.isValid()){
    //std::cout << "MC particles size " << genParticles->size() << std::endl;
    double mcMetX = 0;
    double mcMetY = 0;
    for(size_t i = 0; i < genParticles->size(); ++ i){
	  const reco::GenParticle & part = (*genParticles)[i];
      //Store pid and status of all the particles
	  int id = part.pdgId();
	  MyMCParticle mcParticle;
      //outgoing nutrino
	  if(part.status() == 1 && (abs(id) == 12 || abs(id) == 14 || abs(id) == 16)){
	    mcMetX += part.px();
	    mcMetY += part.py();
      }
      //status=3 does not exit in Pythia8
      //https://indico.cern.ch/event/402279/contributions/960421/attachments/805964/1104514/mcaod-Jun17-2015.pdf
      if((part.status() == 2 && abs(id) < 6 && part.pt() > 10) || (part.status() == 1 && abs(id) < 17 && part.pt() > 10)){ 
	  // searching parents
        int myMCMotherLine = -1;
        std::vector<int> motherList;
        if(part.numberOfMothers() > 0){
          const reco::Candidate *mother = part.mother();
          myMCMotherLine = findMother(mcParticles,mother);
          for(size_t im = 0; im < part.numberOfMothers(); im++){
            int motherId = part.mother(im)->pdgId();
            motherList.push_back(motherId);
          }
        }
        mcParticle.pid    = id;
        mcParticle.status = part.status();
        mcParticle.p4Gen.SetCoordinates(part.px(), part.py(), part.pz(), part.energy());
        if(motherList.size() > 0){
          mcParticle.motherLine = myMCMotherLine;
          mcParticle.mother = motherList;
        }
        mcParticles.push_back(mcParticle);
	  }
    }
    mcMET.p4.SetCoordinates(mcMetX, mcMetY, 0, sqrt(mcMetX*mcMetX + mcMetY*mcMetY));
  }
  //std::cout<<"stored mc particles =  "<<mcParticles.size()<<std::endl;
  return mcParticles;
}

int MyEventSelection::findMother(std::vector<MyMCParticle> mcParticles, const reco::Candidate *iMother){

  int id    = iMother->pdgId();
  double px = iMother->px();
  double py = iMother->py();
  double pz = iMother->pz();
  
  int theMotherLine = -1;
  
  int counter = -1;
  std::vector<MyMCParticle>::const_iterator iMyMCMother = mcParticles.begin();
  while(theMotherLine < 0 && iMyMCMother != mcParticles.end()){
    counter++;
    double R = sqrt( (iMyMCMother->p4Gen.Px() - px)*(iMyMCMother->p4Gen.Px() - px)  + (iMyMCMother->p4Gen.Py() - py)*(iMyMCMother->p4Gen.Py() - py)  + (iMyMCMother->p4Gen.Pz() - pz)*(iMyMCMother->p4Gen.Pz() - pz)  );
    if(R < 0.0001 && iMyMCMother->pid == id ) theMotherLine = counter;
    iMyMCMother++;
  }
  return theMotherLine;
}

SampleInfo MyEventSelection::getSampleInfo(const edm::Event& iEvent, const edm::EventSetup& iSetup){

  SampleInfo mcInfo;
  //----------------------------------------------
  //Assign particular channel for different samples
  //e.g DY: mu+, mu- ; DY: e+, e- ; DY: tau+ tau- 
  //----------------------------------------------
  int mcEvtType_ = MyEvent::OTHER;
  
  //Assign t tbar 
  if(inputDataSampleCode_ == MyEvent::TTJets ){
    mcEvtType_ = assignTTEvent(iEvent,iSetup); 
  }
  //Assign DY 
  else if (inputDataSampleCode_ == MyEvent::DYJetsToLL || inputDataSampleCode_ == MyEvent::DY1JetsToLL || inputDataSampleCode_ == MyEvent::DY2JetsToLL || inputDataSampleCode_ == MyEvent::DY3JetsToLL || inputDataSampleCode_ == MyEvent::DY4JetsToLL ) {
    mcEvtType_ = assignDYchannel(iEvent,iSetup);
  }
  //Assign W jets 
  else if (inputDataSampleCode_ == MyEvent::WJetsToLNu || inputDataSampleCode_ == MyEvent::W1JetsToLNu || inputDataSampleCode_ == MyEvent::W2JetsToLNu || inputDataSampleCode_ == MyEvent::W3JetsToLNu || inputDataSampleCode_ == MyEvent::W4JetsToLNu ){
    mcEvtType_ = assignWJets(iEvent,iSetup);   
  }
  //Assign other
  else mcEvtType_ = inputDataSampleCode_ ;
  mcInfo.mcEvtType = mcEvtType_;
  mcInfo.sampleName = configParamsMC_.getParameter<std::string>("sampleCode");
  
  //----------------------------------------------
  //store GenPileup info
  //----------------------------------------------
  double npuVertices_=0, nOOTpuVertices_=0;
  double nTruePuVertices_=0, nTrueOOTPuVertices_=0;
  edm::Handle<std::vector<PileupSummaryInfo> > puInfoH;
  iEvent.getByToken(PUInfoTag_, puInfoH);
 
  if(puInfoH.isValid()){ 
    //std::cout << "valid puInfoH = " << std::endl;
    double nOOTpuVertices(0),npuVertices(0);
    double nTrueOOTPuVertices(0),nTruePuVertices(0);
    for(std::vector<PileupSummaryInfo>::const_iterator it =puInfoH->begin(); it != puInfoH->end(); it++){
	  if(it->getBunchCrossing() ==0){
	    npuVertices += it->getPU_NumInteractions();
	    nTruePuVertices += it->getTrueNumInteractions();
	  }
	  else{
	    nOOTpuVertices += it->getPU_NumInteractions();
	    nTrueOOTPuVertices += it->getTrueNumInteractions();
	  }
	}
    npuVertices_ = npuVertices;
    nOOTpuVertices_ = nOOTpuVertices;
    nTruePuVertices_ = nTruePuVertices;
    nTrueOOTPuVertices_ = nTrueOOTPuVertices;
    // std::cout<<"npuVertices_ =  "<<npuVertices_<<std::endl;
  }

  std::vector<double>pu; pu.clear();
  pu.push_back(npuVertices_);
  pu.push_back(nOOTpuVertices_);

  std::vector<double>putrue; putrue.clear(); 
  putrue.push_back(nTruePuVertices_); 
  putrue.push_back(nTrueOOTPuVertices_);

  mcInfo.pileup = pu;
  mcInfo.truepileup = putrue;

  myhistos_["intimepu"]->Fill(npuVertices_);
  myhistos_["outoftimepu"]->Fill(nOOTpuVertices_);
  myhistos_["totalpu"]->Fill(npuVertices_+nOOTpuVertices_);
  myhistos_["trueintimepu"]->Fill(nTruePuVertices_); 
  myhistos_["trueoutoftimepu"]->Fill(nTrueOOTPuVertices_); 
  myhistos_["truetotalpu"]->Fill(nTruePuVertices_+nTrueOOTPuVertices_);

 
  //----------------------------------------------
  //get total number of particles (in, inter, out) 
  //in an event at gen level. It is called hepNuP
  //----------------------------------------------
  int hepNUP_ = -99;
  std::vector<int> hepIDUP_;
  std::vector<int> hepISTUP_;
  edm::Handle<LHEEventProduct > LHEHandle;
  const LHEEventProduct* LHE = 0;
  iEvent.getByToken(externalLHEProducer_, LHEHandle);
  if(LHEHandle.isValid()){
    LHE = LHEHandle.product();
    hepNUP_ = (LHE->hepeup()).NUP;
    for(size_t i=0; i<(LHE->hepeup()).IDUP.size(); i++){
      hepIDUP_.push_back((LHE->hepeup()).IDUP[i]);
      hepISTUP_.push_back((LHE->hepeup()).ISTUP[i]);
    }
  }
  mcInfo.hepNUP = hepNUP_;
  mcInfo.hepIDUP = hepIDUP_;
  mcInfo.hepISTUP = hepISTUP_;
  //----------------------------------------------
  //add weights for top pT re-weighting
  //----------------------------------------------
  edm::Handle<reco::GenParticleCollection> genParticles;
  try{
    iEvent.getByToken(GenParticle_, genParticles); //76x
  }catch(...) {;}

  if(genParticles.isValid()){
    double pt_top = -1.0; double pt_antitop = -1.0;
    for(size_t i = 0; i < genParticles->size(); ++ i){
      const reco::GenParticle & part = (*genParticles)[i];
	  //int id = part.pdgId();
      //if(part.status() == 3 &&  part.pdgId() == 6)
      if(part.pdgId() == 6)
	    pt_top = part.pt();
	  //else if(part.status() == 3 &&  part.pdgId() == -6)
	  else if(part.pdgId() == -6)
	    pt_antitop = part.pt();
    }
    if(pt_top >= 0 && pt_antitop >= 0){
      ///double topPtWeight_LJ = sqrt(exp(0.159 - 0.00141*pt_top)*exp(0.159 - 0.00141*pt_antitop));
      ///double topPtWeight_2L = sqrt(exp(0.148 - 0.00129*pt_top)*exp(0.148 - 0.00129*pt_antitop));
      //https://twiki.cern.ch/twiki/bin/viewauth/CMS/TopPtReweighting
      double topPtWeight_comb = sqrt(exp(0.0615 - 0.0005*pt_top)*exp(0.0615 - 0.0005*pt_antitop));
      std::vector<double>topPtW; topPtW.clear();
      ///topPtW.push_back(topPtWeight_LJ);
      ///topPtW.push_back(topPtWeight_2L);
      topPtW.push_back(topPtWeight_comb);
      mcInfo.topPtWeights = topPtW;
    }
  }
  
  return mcInfo;
}

//----------------------------------------------
//Assign DY Channel
//----------------------------------------------
int MyEventSelection::assignDYchannel(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  //retrieve the generator level particle collection
  edm::Handle<reco::GenParticleCollection> genParticles;
  iEvent.getByToken(GenParticle_, genParticles); // 76x
  int dyChannel=MyEvent::ZUNKNOWN;
  
  //iterate over the collection and store Z/gamma->ll decays
  for(size_t i = 0; i < genParticles->size(); ++ i){
    const reco::GenParticle & p = (*genParticles)[i];
    int id_p   = p.pdgId();
    //select Z
    if(abs(id_p) != 23 && abs(id_p)!=22) continue;
    ///if( p.status()!=3) continue;
    int nElecs(0), nMuons(0), nTaus(0), nQuarks(0);
    for(size_t b = 0; b < p.numberOfDaughters(); ++ b){
	  int id_d = abs (p.daughter(b)->pdgId());
	  if(id_d == 11) { nElecs++; }
	  if(id_d == 13) { nMuons++; }
	  if(id_d == 15) { nTaus++;  }
	  if(id_d <= 5 && id_d != 0) { nQuarks++; }
    }
    if(nElecs>1) dyChannel=MyEvent::ZEE;
    else if(nMuons>1) dyChannel=MyEvent::ZMUMU;
    else if(nTaus>1) dyChannel=MyEvent::ZTAUTAU;
    else if(nQuarks>1) dyChannel=MyEvent::ZQQ;
  }
  return dyChannel;
}

//----------------------------------------------
//Assign W Jets Channel
//----------------------------------------------
int MyEventSelection::assignWJets(const edm::Event& iEvent, const edm::EventSetup& iSetup){

  //retrieve the generator level particle collection
  edm::Handle<reco::GenParticleCollection> genParticles;
  iEvent.getByToken(GenParticle_, genParticles); // 76x
  //reset all
  int quarkCounter(0),chLeptonCounter(0),electronCounter(0),muonCounter(0),tauCounter(0);
  for(size_t i = 0; i < genParticles->size(); ++ i) {
    const reco::GenParticle & p = (*genParticles)[i];
    int id_p   = p.pdgId();
    if( abs(id_p) == 24 ){      //If we find a W
	  for(size_t b = 0; b < p.numberOfDaughters(); ++ b){
	    const reco::Candidate * Wson_i = p.daughter(b);
	    int id_Wson = Wson_i->pdgId();
	    //taus must be traced further
	    if(abs(id_Wson) == 15){
	      int tempLepton = 0;
	      for(size_t c = 0; c < Wson_i->numberOfDaughters(); ++ c){
	        const reco::Candidate * TauSon = Wson_i->daughter(c);
	        if(abs(TauSon->pdgId()) == 15){
		    //cout<<"Tau daughetrs "<<TauSon->numberOfDaughters()<<endl;
		      for(size_t d = 0; d < TauSon->numberOfDaughters(); ++ d){
		        const reco::Candidate * TauSon2 = TauSon->daughter(d);
		        int tausonid = abs(TauSon2->pdgId());
		        //cout<<"tau daughter Id "<<tausonid<<" status "<<TauSon2->status()<<endl;
		        if     (tausonid == 11){tempLepton = 11;}
		        else if(tausonid == 13){tempLepton = 13;}
		      }
	        }
	        else if(abs(TauSon->pdgId()) ==11 ) {tempLepton = 11;}
	        else if(abs(TauSon->pdgId()) ==13 ) {tempLepton = 13;}
	      }
	      if     (tempLepton == 11) { chLeptonCounter++; electronCounter++;  }
	      else if(tempLepton == 13) { chLeptonCounter++; muonCounter++;      }
	      else                      { chLeptonCounter++; tauCounter++;       }
	    }
	    else{
	      switch(abs(id_Wson)){
            case 1:  quarkCounter++;  break;
            case 2:  quarkCounter++;  break;
            case 3:  quarkCounter++;  break;
            case 4:  quarkCounter++;  break;
            case 5:  quarkCounter++;  break;
            case 6:  quarkCounter++;  break;
            case 11: chLeptonCounter++; electronCounter++; break;
            case 13: chLeptonCounter++; muonCounter++;     break;
            case 12: case 14: case 16:  break;
	      }
	    }
	  }
    }
  }
    
  int  wChannel = MyEvent::WUNKNOWN;
  if     (quarkCounter == 2)                          { wChannel = MyEvent::WQQ;    }
  else if(quarkCounter == 0 && electronCounter == 1 ) { wChannel = MyEvent::WENU;   }
  else if(quarkCounter == 0 && muonCounter == 1     ) { wChannel = MyEvent::WMUNU;  }
  else if(quarkCounter == 0 && tauCounter == 1      ) { wChannel = MyEvent::WTAUNU; }
  //cout<<"WChannel =  "<<wChannel<<endl;
  return wChannel;
}

//----------------------------------------------
//Assign t tbar channel
//----------------------------------------------
int MyEventSelection::assignTTEvent(const edm::Event& iEvent, const edm::EventSetup& iSetup){

  //reset all
  int quarkCounter(0),chLeptonCounter(0),electronCounter(0),muonCounter(0),tauCounter(0);
  //retrieve the generator level particle collection
  edm::Handle<reco::GenParticleCollection> genParticles;
  iEvent.getByToken(GenParticle_, genParticles); // 76x
  
  //iterate over the collection and store t->Wb decays
  std::vector<const reco::Candidate*> W_Sons;
  for(size_t i = 0; i < genParticles->size(); ++i) {
    const reco::GenParticle & p = (*genParticles)[i];
    int id_p   = p.pdgId();
    //select tops
    if(abs(id_p) != 6) continue;
    for(size_t b = 0; b < p.numberOfDaughters(); ++ b){
      const reco::Candidate * top_daughter = p.daughter(b);
      //store W daughters
      if(abs(top_daughter->pdgId()) == 24){
        for(size_t c = 0; c < top_daughter->numberOfDaughters(); ++c){
	      const reco::Candidate * W_daughter = top_daughter->daughter(c);
	      if(abs(W_daughter->pdgId()) != 24 && abs(W_daughter->pdgId()) != 37) W_Sons.push_back(W_daughter);
	    }
	  }
    }
  }
  
  //check W decay chain
  for(std::vector<const reco::Candidate*>::const_iterator Wson_i = W_Sons.begin(); Wson_i != W_Sons.end(); Wson_i++){
    int id_Wson = (*Wson_i)->pdgId();
    //taus must be traced further
    if(abs((*Wson_i)->pdgId()) == 15){
      int tempLepton = 0;
      for(size_t c = 0; c < (*Wson_i)->numberOfDaughters(); ++ c){
        const reco::Candidate * TauSon = (*Wson_i)->daughter(c);
        if(abs(TauSon->pdgId()) == 15){
          for(size_t d = 0; d < TauSon->numberOfDaughters(); ++ d){
            const reco::Candidate * TauSon2 = TauSon->daughter(d);
            int tausonid = abs(TauSon2->pdgId());
            if     (tausonid == 11){tempLepton = 11; }
            else if(tausonid == 13){tempLepton = 13; }
          }
        }
        else if(abs(TauSon->pdgId()) ==11 ) {tempLepton = 11;}
        else if(abs(TauSon->pdgId()) ==13 ) {tempLepton = 13;}
      }
      
      if     (tempLepton == 11) { chLeptonCounter++; electronCounter++; }
      else if(tempLepton == 13) { chLeptonCounter++; muonCounter++; }
      else                      { chLeptonCounter++; tauCounter++; }
    }
    else{
      switch(abs(id_Wson)){
        case 1:  quarkCounter++;  break;
        case 2:  quarkCounter++;  break;
        case 3:  quarkCounter++;  break;
        case 4:  quarkCounter++;  break;
        case 5:  quarkCounter++;  break;
        case 6:  quarkCounter++;  break;
        case 11: chLeptonCounter++; electronCounter++; break;
        case 13: chLeptonCounter++; muonCounter++;     break;
        case 12: case 14: case 16: break;
      }
      
    }
  }
  int ttChannel = MyEvent::TTUNKNOWN;
  if     (quarkCounter == 4) {ttChannel = MyEvent::TTALLJETS;}
  else if(quarkCounter == 2 && electronCounter == 1) {ttChannel = MyEvent::TTEJETS;}
  else if(quarkCounter == 2 && muonCounter == 1) {ttChannel = MyEvent::TTMUJETS;}
  else if(quarkCounter == 2 && tauCounter == 1) {ttChannel = MyEvent::TTTAUJETS;}
  else if(quarkCounter == 0 && electronCounter == 2) {ttChannel = MyEvent::TTEE;}
  else if(quarkCounter == 0 && electronCounter == 1 && muonCounter == 1){ttChannel = MyEvent::TTEMU;}
  else if(quarkCounter == 0 && electronCounter == 1 && tauCounter  == 1){ttChannel = MyEvent::TTETAU;}
  else if(quarkCounter == 0 && muonCounter == 2) {ttChannel = MyEvent::TTMUMU;}
  else if(quarkCounter == 0 && muonCounter == 1 && tauCounter == 1) {ttChannel = MyEvent::TTMUTAU;}
  else if(quarkCounter == 0 && tauCounter == 2) {ttChannel = MyEvent::TTTAUTAU;}
  return ttChannel;
}

