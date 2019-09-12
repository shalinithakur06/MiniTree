#include "MiniTree/Selection/interface/MyEvent.h"
#include "MiniTree/Selection/interface/MyJet.h"
#include "MiniTree/Selection/interface/MyMET.h"
#include "MiniTree/Selection/interface/MyVertex.h"
#include "MiniTree/Selection/interface/MyElectron.h"
#include "MiniTree/Selection/interface/MyMuon.h"
//#include "MiniTree/Selection/interface/MyTau.h"
#include "MiniTree/Selection/interface/SampleInfo.h"
#include "MiniTree/Selection/interface/MomentumVec.h"

#include<vector>
#include "TROOT.h"

namespace{
  namespace{
    std::map<int   , std::vector<int> >        dummy1;
    std::pair<float, std::string>              dummy2;
    std::vector<std::pair<float, std::string> > dummy3;
    MyEvent          Myevt; 
    MyVertex         Myvertex; 
    MyJet            Myjet; 
    MyMET            Mymet; 
    MyElectron       Myele; 
    MyMuon           Mymuon; 
    SampleInfo       sampleinfo;
    std::vector<MyVertex>         Myvertices; 
    std::vector<MyJet>            Myjets; 
    std::vector<MyMET>            Mymets; 
    std::vector<MyElectron>       Myeles; 
    std::vector<MyMuon>           Mymuons; 
    std::vector<ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> > > lvec;
    MyLorentzVector         vec;
    Point3D               point;  
  }
}
