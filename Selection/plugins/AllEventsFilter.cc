#include "MiniTree/Selection/interface/AllEventsFilter.h"

AllEventsFilter::AllEventsFilter (const edm::ParameterSet& pset){

  edm::Service<TFileService> fs;
  totalEvents_ = fs->make<TH1F>("totalEvents","totalEvents",1,0., 1.);
}

AllEventsFilter::~AllEventsFilter(){}


bool AllEventsFilter::filter(edm::Event& iEvent, const edm::EventSetup& iSetup){
  totalEvents_->Fill(0.5);
  return true;
}

//define this as a plug-in
DEFINE_FWK_MODULE(AllEventsFilter);
