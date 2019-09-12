#include "MiniTree/Selection/interface/MyEventSelection.h"

std::vector<std::string> MyEventSelection::getHLT(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  std::vector<std::string> hltPaths;
  hltPaths.clear();

  //  edm::InputTag hlt_ = configParamshlt_.getParameter<edm::InputTag>("source");
  std::vector<std::string> myTrigNames = configParamshlt_.getParameter<std::vector<std::string> >("bits");

  edm::Handle<edm::TriggerResults> hltresults;
  iEvent.getByToken(hlt_, hltresults); // 76x
  ////assert(hltresults.isValid());
  
  const edm::TriggerNames& TrigNames_ = iEvent.triggerNames(*hltresults);
  const int ntrigs = hltresults->size();
  for (int itr=0; itr<ntrigs; itr++){
    if(!hltresults->wasrun(itr) )continue;
    if (!hltresults->accept(itr)) continue;
    
    bool passTrig = false;
    std::string trigName=TrigNames_.triggerName(itr);
    for(unsigned int i = 0; i<myTrigNames.size(); i++){
      if(trigName.find(myTrigNames[i]) != string::npos) passTrig = true;
    }
    if(passTrig) hltPaths.push_back(trigName);
  }
  return hltPaths;
}
