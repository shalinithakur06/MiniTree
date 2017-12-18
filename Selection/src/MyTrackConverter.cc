#include "MiniTree/Selection/interface/MyEventSelection.h"

MyTrack MyEventSelection::myTrackConverter(const reco::TransientTrack& transientTrack){

  const reco::Track recTrack = transientTrack.track();
  return myTrackConverter(recTrack);
}

MyTrack MyEventSelection::myTrackConverter(const reco::Track& recTrack){

  MyTrack track;
  track.p4.SetCoordinates(recTrack.px(), recTrack.py(), recTrack.pz(), recTrack.p());
    track.trackCharge = recTrack.charge();
  track.chiSquared  = recTrack.normalizedChi2();
  track.nHits       = recTrack.numberOfValidHits();
  
  return track;
}


MyTrack MyEventSelection::myTrackConverter(const reco::PFCandidate& pfTrack){
  
  MyTrack track;
  track.p4.SetCoordinates(pfTrack.px(), pfTrack.py(), pfTrack.pz(), pfTrack.p());
  track.trackCharge  = pfTrack.charge();
  track.particleType = pfTrack.particleId();
  
  return track;
}

