#include "MiniTree/Selection/interface/MyEventSelection.h"

std::vector<std::string> MyEventSelection::getHLT(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  std::vector<std::string> hltPaths;
  hltPaths.clear();

  //  edm::InputTag hlt_ = configParamshlt_.getParameter<edm::InputTag>("source");
  std::vector<std::string> myTrigNames = configParamshlt_.getParameter<std::vector<std::string> >("bits");
  std::string myTrig = configParamshlt_.getParameter<std::string>("myTrig");

  edm::Handle<edm::TriggerResults> hltresults;
  iEvent.getByToken(hlt_, hltresults); // 76x
  ////assert(hltresults.isValid());
  
  const edm::TriggerNames& TrigNames_ = iEvent.triggerNames(*hltresults);
  const int ntrigs = hltresults->size();

  if(initTriggerHistos_){
    initTriggerHistos_ = false;
    dirs_.push_back( fs_->mkdir("trigger") );
    myhistos_["trigger_bitsrun"] = dirs_[dirs_.size() - 1].make<TH1D>("trigger_bitsrun", "trigger run", ntrigs, 0, ntrigs);
    myhistos_["trigger_bitsaccept"] = dirs_[dirs_.size() - 1].make<TH1D>("trigger_bitsaccept", "trigger accept", ntrigs, 0, ntrigs);
    for(int itrig=0; itrig<ntrigs; itrig++)
      {
	std::string trigName = TrigNames_.triggerName(itrig);
	myhistos_["trigger_bitsrun"]->GetXaxis()->SetBinLabel(itrig+1,trigName.c_str());
	myhistos_["trigger_bitsaccept"]->GetXaxis()->SetBinLabel(itrig+1,trigName.c_str());
      }
  }
  
  for (int itr=0; itr<ntrigs; itr++){
    if(!hltresults->wasrun(itr) )continue;
    myhistos_["trigger_bitsrun"]->Fill(itr);
    if (!hltresults->accept(itr)) continue;
    myhistos_["trigger_bitsaccept"]->Fill(itr);
    
    bool passTrig = false;
    std::string trigName=TrigNames_.triggerName(itr);
    if(trigName.find(myTrig) != string::npos) passTrig = true;
    /*
    if(myTrigNames.size() > 0){
      if(find(myTrigNames.begin(), myTrigNames.end(), trigName) == myTrigNames.end()) continue = true;
    }
    */
    if(passTrig) hltPaths.push_back(trigName);
  }
  return hltPaths;
}
