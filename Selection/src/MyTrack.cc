#include "MiniTree/Selection/interface/MyTrack.h"

MyTrack::MyTrack():
  trackCharge(0),
  chiSquared(9.), 
  nHits(-9.),
  nPixHits(-9.),
  particleType(-9.),
  d0(-9.),
  d0Error(-9.)
{
}

MyTrack::~MyTrack(){
}


void MyTrack::Reset(){
  
  p4.SetCoordinates(0.0,0.0,0.0,0.0);
  
  trackCharge = 0;
  chiSquared = 9.;
  nHits = -9.;
  nPixHits = -9.;
  particleType = -9.;
  d0 = -9.;
  d0Error = -9.;
  
}
