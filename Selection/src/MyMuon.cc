#include "MiniTree/Selection/interface/MyMuon.h"

MyMuon::MyMuon():

  ///basic
  charge(0),
  gen_id(0),
  gen_mother_id(0),
  muName(""),
  type(0),
  
  ///id
  //Loose
  isGlobalMuon(true),
  isPFMuon(true),
  isTrackerMuon(true),
  
  //Medium
  chi2LocalPosition(-9.),
  normChi2(-9.),
  segmentCompatibility(-9.),
  trkKink(-9.),
  validFraction(-9.),
 
  //Tight
  D0(-9.),
  Dz(-9.),
  nMatchedStations(-9),
  nMuonHits(-9),
  nPixelHits(-9),
  nTrackerLayers(-9),
  
  ///iso
  ChHadIso(-9.),  
  NeuHadIso(-9.),   
  pfRelIso(-9.), 
  PhotonIso(-9.),   
  PileupIso(-9.), 
  trigger_mu_pt(0),
  quality(0)
{
}

MyMuon::~MyMuon()
{
}

void MyMuon::Reset()
{
  
  ///basic
  charge = 0;
  gen_id = 0;
  gen_mother_id = 0;
  muName = "";
  p4.SetCoordinates(0.0, 0.0, 0.0, 0.0);
  type = 0;
  vertex.SetCoordinates(0.0, 0.0, 0.0);
  
  ///id
  //Loose
  isGlobalMuon = true;
  isPFMuon = true;
  isTrackerMuon = true;
  //Medium
  chi2LocalPosition = -9.;
  normChi2 = -9.;
  segmentCompatibility = -9.;
  trkKink = -9.;
  validFraction = -9.;
  //Tight
  D0 = -9.;
  Dz = -9.;
  nMatchedStations = -9;
  nMuonHits = -9;
  nPixelHits = -9;
  nTrackerLayers = -9;
  
  ///iso
  ChHadIso = -9.;   
  NeuHadIso = -9.;    
  pfRelIso = -9.;  
  PhotonIso = -9.;    
  PileupIso = -9.;  
  trigger_mu_pt = 0;
  quality = 0;
}
