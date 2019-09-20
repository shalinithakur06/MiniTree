#ifndef __MYMUON_H__
#define __MYMUON_H__

#include "TROOT.h"
#include <map>
#include <vector>
#include <string>

#include "MomentumVec.h"


class MyMuon 
{
 public:
  MyMuon();
  ~MyMuon();
  
  void Reset();
  
  ///std::string GetName() { return name; };

  //https://twiki.cern.ch/twiki/bin/viewauth/CMS/SWGuideMuonIdRun2

  ///basic
  int charge;
  int gen_id;
  int gen_mother_id;
  //std::string name;
  TString muName;
  MyLorentzVector p4; 
  MyLorentzVector Genp4; 
  int type;
  Point3D  vertex;   

  ///id
  //Loose
  bool isGlobalMuon;
  bool isPFMuon;
  bool isTrackerMuon;
  //Medium 
  double chi2LocalPosition;
  double normChi2;
  double segmentCompatibility;
  double trkKink;
  double validFraction;
  //Tight
  double D0;          
  double Dz;
  int nMatchedStations;
  int nMuonHits;
  int nMuonHitsTuneP;
  int nPixelHits;
  int nTrackerLayers;
  
  //High Pt
  double bestMuPtErr; 
  double bestMuPtTrack;

  ///iso
  double ChHadIso; 
  double NeuHadIso;  
  double PhotonIso;  
  double PileupIso;
  double pfRelIso;
  double trkRelIso;
  double expectedMatchedStations;
  double nStationMask;
  double nRPCLayers;
 private :

};
#endif
