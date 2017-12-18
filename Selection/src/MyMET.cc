#include "MiniTree/Selection/interface/MyMET.h"

MyMET::MyMET():
  
  emEtFraction(-9.),
  hadEtFraction(-9.),
  isPFMET(false),
  metName(""),
  metSignificance(-9.),
  muonEtFraction(-9.),
  sumEt(-9.)
{
}

MyMET::~MyMET()
{
}
void MyMET::Reset()
{
  emEtFraction = -9.;
  hadEtFraction = -9.;
  isPFMET = false;
  metName = "";
  metSignificance = -9.;
  muonEtFraction = -9.;
  p4.SetCoordinates(0.0,0.0,0.0,0.0);
  sumEt = -9.;
}
