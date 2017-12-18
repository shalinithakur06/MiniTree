#include "MiniTree/Selection/interface/MyEventSelection.h"
#include "TrackingTools/TransientTrack/interface/TransientTrack.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"
#include "TrackingTools/IPTools/interface/IPTools.h"
#include "DataFormats/JetReco/interface/JPTJet.h"

MyEventSelection::MyEventSelection(const edm::ParameterSet& iConfig, edm::ConsumesCollector&& cc)  
{
  configParamsVertex_ = iConfig.getParameter<edm::ParameterSet>("Vertex");
  configParamsJets_ = iConfig.getParameter<edm::ParameterSet>("Jets");
  configParamsMETs_ = iConfig.getParameter<edm::ParameterSet>("Mets");
  configParamsMuons_ = iConfig.getParameter<edm::ParameterSet>("Muons");
  configParamsElectrons_ = iConfig.getParameter<edm::ParameterSet>("Electrons");
  configParamshlt_ = iConfig.getParameter<edm::ParameterSet>("Trigger");
  configParamsMC_ = iConfig.getParameter<edm::ParameterSet>("MCTruth");
  
  // vertex
  vtxSource = cc.consumes<reco::VertexCollection>(configParamsVertex_.getParameter<edm::InputTag>("vertexSource"));
  bsSource = cc.consumes<reco::BeamSpot>(configParamsVertex_.getParameter<edm::InputTag>("beamSpotSource"));
  rhoSource = cc.consumes<double>( configParamsVertex_.getParameter<edm::InputTag>("rho"));

  // Muon
  Muonsources = cc.consumes<pat::MuonCollection>(configParamsMuons_.getParameter<edm::InputTag>("sources")); 
  TrigEvent_ = cc.consumes<pat::TriggerEvent>(configParamsJets_.getParameter<edm::InputTag>("triggerEvent"));

  // Elctrons
  Elesources = cc.consumes<pat::ElectronCollection>(configParamsElectrons_.getParameter<edm::InputTag>("sources"));
  //EleConversion_ = cc.consumes<reco::ConversionCollection>(edm::InputTag("reducedEgamma"));
  eventrhoToken_ = cc.consumes<double>(configParamsElectrons_.getParameter<edm::InputTag>("rhoIso"));
  beamSpotToken_ = cc.consumes<reco::BeamSpot>(configParamsElectrons_.getParameter<edm::InputTag>("beamSpot"));
  jetIDMapToken_ = cc.consumes<reco::JetIDValueMap>(edm::InputTag("ak5JetID"));
  conversionsMiniAODToken_ = cc.consumes<reco::ConversionCollection>(configParamsElectrons_.getParameter<edm::InputTag>("conversionsMiniAOD"));
 

  // Jets
  Jetsources = cc.consumes<pat::JetCollection>(configParamsJets_.getParameter<edm::InputTag>("sources"));
  m_rho_token  = cc.consumes<double>(configParamsJets_.getParameter<edm::InputTag>("jet_rho"));
  //local .txt file
  m_resolutions_file = configParamsJets_.getParameter<std::string>("resolutionsFile");
  m_scale_factors_file = configParamsJets_.getParameter<std::string>("scaleFactorsFile");
  //.txt files from Central repo
  //m_resolutions_file = configParamsJets_.getParameter<edm::FileInPath>("resolutionsFile").fullPath();
  //m_scale_factors_file = configParamsJets_.getParameter<edm::FileInPath>("scaleFactorsFile").fullPath();
  //TrigEvent_ = cc.consumes<pat::TriggerObjectStandAloneCollection>(configParamsJets_.getParameter<edm::InputTag>("triggerEvent"));

  // Mets
  Metsources = cc.consumes<pat::METCollection>(configParamsMETs_.getParameter<edm::InputTag>("sources"));

  // Trigger
  hlt_ = cc.consumes<edm::TriggerResults>(configParamshlt_.getParameter<edm::InputTag>("source"));

  // MC and PU
  PUInfoTag_ = cc.consumes<vector<PileupSummaryInfo> >(edm::InputTag("slimmedAddPileupInfo"));
  GenParticle_ = cc.consumes<reco::GenParticleCollection>(edm::InputTag("prunedGenParticles"));
  externalLHEProducer_ = cc.consumes<LHEEventProduct>(edm::InputTag("externalLHEProducer"));
 
  std::string code = configParamsMC_.getParameter<std::string>("sampleCode");
  cout<<"code = "<<code<<endl;
  inputch = configParamsMC_.getParameter<std::string>("sampleChannel");
  if(code!=std::string("DATA")) { isData_=false; }
  else{ isData_=true; inputDataSampleCode_ = MyEvent::DATA; }
  if(code==std::string("DY1JetsToLL")) { inputDataSampleCode_   = MyEvent::DY1JetsToLL;}
  if(code==std::string("DY2JetsToLL")) { inputDataSampleCode_   = MyEvent::DY2JetsToLL;}
  if(code==std::string("DY3JetsToLL")) { inputDataSampleCode_   = MyEvent::DY3JetsToLL;}
  if(code==std::string("DY4JetsToLL")) { inputDataSampleCode_   = MyEvent::DY4JetsToLL;}
  if(code==std::string("DYJetsToLL")) { inputDataSampleCode_    = MyEvent::DYJetsToLL;}
  if(code==std::string("HplusM100")) { inputDataSampleCode_     = MyEvent::HplusM100;}
  if(code==std::string("HplusM120")) { inputDataSampleCode_     = MyEvent::HplusM120;}
  if(code==std::string("HplusM140")) { inputDataSampleCode_     = MyEvent::HplusM140;}
  if(code==std::string("HplusM150")) { inputDataSampleCode_     = MyEvent::HplusM150;}
  if(code==std::string("HplusM160")) { inputDataSampleCode_     = MyEvent::HplusM160;}
  if(code==std::string("HplusM80")) { inputDataSampleCode_      = MyEvent::HplusM80;}
  if(code==std::string("HplusM90")) { inputDataSampleCode_      = MyEvent::HplusM90;}
  if(code==std::string("QCD_Pt_15to20")) {inputDataSampleCode_  = MyEvent::QCD_Pt_15to20;} 
  if(code==std::string("QCD_Pt_20to30")) {inputDataSampleCode_  = MyEvent::QCD_Pt_20to30;}
  if(code==std::string("QCD_Pt_30to50")) {inputDataSampleCode_  = MyEvent::QCD_Pt_30to50;}
  if(code==std::string("QCD_Pt_50to80")) {inputDataSampleCode_  = MyEvent::QCD_Pt_50to80;}
  if(code==std::string("QCD_Pt_80to120")) {inputDataSampleCode_ = MyEvent::QCD_Pt_80to120;}
  if(code==std::string("QCD_Pt_120to170")) {inputDataSampleCode_= MyEvent::QCD_Pt_120to170;}
  if(code==std::string("QCD_Pt_170to300")) {inputDataSampleCode_= MyEvent::QCD_Pt_170to300;}
  if(code==std::string("QCD_Pt_300to470")) {inputDataSampleCode_= MyEvent::QCD_Pt_300to470;}
  if(code==std::string("ST_s")) { inputDataSampleCode_          = MyEvent::ST_s;}
  if(code==std::string("ST_t")) { inputDataSampleCode_          = MyEvent::ST_t;}
  if(code==std::string("ST_tW")) { inputDataSampleCode_         = MyEvent::ST_tW;}
  if(code==std::string("TTJets")){ inputDataSampleCode_         = MyEvent::TTJets;}
  if(code==std::string("W1JetsToLNu")) { inputDataSampleCode_   = MyEvent::W1JetsToLNu;}
  if(code==std::string("W2JetsToLNu")) { inputDataSampleCode_   = MyEvent::W2JetsToLNu;}
  if(code==std::string("W3JetsToLNu")) { inputDataSampleCode_   = MyEvent::W3JetsToLNu;}
  if(code==std::string("W4JetsToLNu")) { inputDataSampleCode_   = MyEvent::W4JetsToLNu;}
  if(code==std::string("WJetsToLNu")) { inputDataSampleCode_    = MyEvent::WJetsToLNu;}
  if(code==std::string("WW"))   { inputDataSampleCode_          = MyEvent::WW;}
  if(code==std::string("WZ"))   { inputDataSampleCode_          = MyEvent::WZ;}
  if(code==std::string("ZZ"))   { inputDataSampleCode_          = MyEvent::ZZ;}
  if(code==std::string("")) { inputDataSampleCode_              = MyEvent::OTHER;}
}

MyEventSelection::~MyEventSelection()
{

}

void MyEventSelection::Set(const edm::Event& e, const edm::EventSetup& es)
{

  ///es.get<TransientTrackRecord>().get("TransientTrackBuilder", trackBuilder);
  event_.eventNb = e.id().event();
  event_.lumiblock = e.luminosityBlock();
  event_.isData = isData_;
  
  //get trigger, vertex, muon, electron, jet, met
  event_.hlt = getHLT(e, es);
  event_.PrimaryVtxs = getVertices(e, es);
  event_.Muons = getMuons(e, es);
  event_.Electrons = getElectrons(e, es);
  event_.Jets = getJets(e, es);
  event_.mets = getMETs(e, es);
  //std::cout<<"pass met "<<event_.mets.size()<<std::endl;
  
  if(!isData_){
    event_.mcParticles = getMCParticles(e);
    event_.mcMET = mcMET;
    event_.sampleInfo = getSampleInfo(e, es);
  }

  //make event selection
  bool passTrig = false;
  std::vector<std::string> trigs = event_.hlt;
  for(size_t itrig = 0; itrig < trigs.size(); itrig++){
    if(trigs[itrig].find("Ele") != std::string::npos){passTrig = true;}//cout <<"ele trig passed"<<endl;}
    if(trigs[itrig].find("Mu") != std::string::npos){passTrig = true;}//cout<<"mu trig passed"<<endl;}
  }
  ///Electrons
  int nIsoMuon = 0, nIsoElectron = 0;
  std::vector<MyElectron> electrons = event_.Electrons;
  for(size_t iele = 0; iele < electrons.size(); iele++){
    //std::string algo = electrons[iele].eleName;
    std::string algo(electrons[iele].eleName);
    //if(algo.find("PFlow") == std::string::npos) continue;
    bool passKin = false, passId = false, passIso = false;
    int quality = electrons[iele].quality;
    if(quality & 0x1)passKin = true;
    if((quality >> 1) & 0x1)passId = true;
    if((quality >> 2) & 0x1)passIso = true;
    if(passKin && passId && passIso){
     myhistos_["SelElePt"]->Fill(electrons[iele].p4.Pt());
     myhistos_["SelEleEta"]->Fill(electrons[iele].p4.Eta());
      nIsoElectron++;
    }
  }
  myhistos_["SelEleMultiplicity"]->Fill(nIsoElectron);
  
  ///Muons
  std::vector<MyMuon> muons = event_.Muons;
  for(size_t imu = 0; imu < muons.size(); imu++){
    //std::string algo = muons[imu].muName;
    std::string algo(muons[imu].muName);
    //if(algo.find("PFlow") == std::string::npos) continue;
    bool passKin = false, passId = false, passIso = false;
    int quality = muons[imu].quality;
    if(quality & 0x1)passKin = true;
    if((quality >> 1) & 0x1)passId = true;
    if((quality >> 2) & 0x1)passIso = true;
    if(passKin && passId && passIso){
    ///if(passKin && passId){
      myhistos_["SelMuPt"]->Fill(muons[imu].p4.Pt());
      myhistos_["SelMuEta"]->Fill(muons[imu].p4.Eta());
      nIsoMuon++;
    }
  }
  myhistos_["SelMuMultiplicity"]->Fill(nIsoMuon);

  int nIsoLepton = 0;
  //  int nIsoLepton = nIsoMuon + nIsoElectron;
  if(inputch==std::string("electron")){ nIsoLepton = nIsoElectron;/* std::cout << "elctron found" << std::endl;*/}
  //  int nIsoLepton = nIsoElectron;
  if(inputch==std::string("muon")){ nIsoLepton = nIsoMuon; /*std::cout << "muon found" << std::endl;*/}

  ///Jets
  std::vector<MyJet> jets = event_.Jets;
  int nJets = 0, nHighPtJets = 0;
  for(size_t ijet = 0; ijet < jets.size(); ijet++){
    std::string algo(jets[ijet].jetName);
    ///if(isData_ && algo.find("ResCor") == std::string::npos) continue;
    bool passKin = false, passId = false;
    int quality = jets[ijet].quality;
    if(quality & 0x1)passKin = true;
    if((quality >> 1) & 0x1)passId = true;
    if(passKin && passId){
      myhistos_["SelJetPt"]->Fill(jets[ijet].p4.Pt());
      myhistos_["SelJetEta"]->Fill(jets[ijet].p4.Eta());
      nJets++;
      if(jets[ijet].p4.Pt() > 30)nHighPtJets++;
    }
  }
  myhistos_["SelJetMultiplicity"]->Fill(nJets);

  int EventQuality = 0;
  if(passTrig){
    EventQuality++;
    if(nIsoLepton > 0){
      EventQuality++;
      if(nJets > 2){
	EventQuality++;
	if(nHighPtJets > 0){
	  EventQuality++;
	  if(nHighPtJets > 1){
	    EventQuality++;
	  }
	}
      }
    }
  }

  for(int istep = 0; istep <= EventQuality; istep++){
    myhistos_["cutflow"]->Fill(istep);
  }

  event_.eventQuality = EventQuality;
  fs_->cd();
}


//void MyEventSelection::BookHistos(edm::Service<TFileService> tfs_)
void MyEventSelection::BookHistos()
{
  ///book histograms
  initTriggerHistos_ = true;
  
  //selection
  dirs_.push_back( fs_->mkdir("selection") );
  myhistos_["cutflow"] = dirs_[dirs_.size() - 1].make<TH1D>("cutflow", "cutflow", 10, 0., 10.);
  //myhistos2_["cutflowmctruth"] = dirs_[dirs_.size() - 1].make<TH2D>("cutflowmctruth", "cutflow", 10, 0., 10., 18, 0., 18.);
  TString steps[6] = {"reco","trigger","#geq 1 leptons","#geq 1 jet","#geq 1 jet (pT > 25)", "#geq 2 jets (pT > 25)"};
  TString ttch[18] = {"unk.", "full had","e+jets","#mu+jets","#tau+jets","ee","e#mu","e#tau","#mu#mu","#mu#tau","#tau#tau", "z+jets","z#tau#tau","w+jets","top-s", "top-t", "top-w","qcd"};
  const size_t nsteps = sizeof(steps)/sizeof(TString);
  for(uint istep=0; istep<nsteps; istep++ ){
    //myhistos2_["cutflowmctruth"]->GetXaxis()->SetBinLabel(istep+1, steps[istep]);
    myhistos_["cutflow"]->GetXaxis()->SetBinLabel(istep+1, steps[istep]);
  }
  //for(int ich=0;   ich<18;  ich++  ){
  //  myhistos2_["cutflowmctruth"]->GetYaxis()->SetBinLabel(ich+1, ttch[ich]);
  //}

  myhistos_["SelJetPt"]  = dirs_[dirs_.size() - 1].make<TH1D>("SelJetPt", "jet Pt",200, 0, 400.);
  myhistos_["SelJetEta"]  = dirs_[dirs_.size() - 1].make<TH1D>("SelJetEta", "jet Eta;jet #eta;N_{events}",100, -5.0, 5.0);
  myhistos_["SelJetMultiplicity"]  = dirs_[dirs_.size() - 1].make<TH1D>("SelJetMultiplicity", "jet Multi",20, 0, 20);
  myhistos_["SelElePt"]  = dirs_[dirs_.size() - 1].make<TH1D>("SelElePt", "electron Pt",200, 0, 400.);
  myhistos_["SelEleEta"]  = dirs_[dirs_.size() - 1].make<TH1D>("SelEleEta", "ele Eta",100, -5.0, 5.0);
  myhistos_["SelEleMultiplicity"]  = dirs_[dirs_.size() - 1].make<TH1D>("SelEleMultiplicity", "electron Multi",20, 0, 20);
  myhistos_["SelMuPt"]  = dirs_[dirs_.size() - 1].make<TH1D>("SelMuPt", "muon Pt",200, 0, 400.);
  myhistos_["SelMuEta"]  = dirs_[dirs_.size() - 1].make<TH1D>("SelMuEta", "muon Eta;" ,100, -5.0, 5.0);
  myhistos_["SelMuMultiplicity"]  = dirs_[dirs_.size() - 1].make<TH1D>("SelMuMultiplicity", "muon Multi;",20, 0, 20);

  ///MC
  dirs_.push_back( fs_->mkdir("MCINFO") );
  myhistos_["intimepu"] = dirs_[dirs_.size() - 1].make<TH1D>("intimepu", "intime pileup", 6000, 0, 1000);
  myhistos_["outoftimepu"] = dirs_[dirs_.size() - 1].make<TH1D>("outoftimepu", "out of time pileup", 6000, 0, 1000);
  myhistos_["totalpu"] = dirs_[dirs_.size() - 1].make<TH1D>("totalpu", "total pileup", 6000, 0, 1000);
  myhistos_["trueintimepu"] = dirs_[dirs_.size() - 1].make<TH1D>("trueintimepu", "intime pileup", 6000, 0., 1000.); 
  myhistos_["trueoutoftimepu"] = dirs_[dirs_.size() - 1].make<TH1D>("trueoutoftimepu", "out of time pileup",6000,0.,1000.); 
  myhistos_["truetotalpu"] = dirs_[dirs_.size() - 1].make<TH1D>("truetotalpu", "total pileup", 6000, 0., 1000.);

  ///Jets
  TString jetrawtag="Jets";
  std::string jettag(jetrawtag);
  //Make a new TDirectory
  dirs_.push_back( fs_->mkdir(jettag.c_str()) );
  myhistos_["pt_"+jetrawtag]  =  dirs_[dirs_.size() - 1].make<TH1D>("pt_"+jetrawtag, "Jet Pt", 200, 0., 500.);
  myhistos_["eta_"+jetrawtag] = dirs_[dirs_.size() - 1].make<TH1D>("eta_"+jetrawtag, "Jet #eta", 100, -5.0, 5.0);
  myhistos_["phi_"+jetrawtag] = dirs_[dirs_.size() - 1].make<TH1D>("phi_"+jetrawtag, "Jet #phi", 80, -4.05, 3.95);
  myhistos_["emf_"+jetrawtag] = dirs_[dirs_.size() - 1].make<TH1D>("emf_"+jetrawtag, "Jet emf", 120, 0, 1.02);
  myhistos_["JER_"+jetrawtag]  = dirs_[dirs_.size() - 1].make<TH1D>("JER_"+jetrawtag, "Jet resolution", 100, 0, 5);
  ////https://github.com/rappoccio/usercode/blob/Dev_53x/EDSHyFT/plugins/BTaggingEffAnalyzer.cc
  double ptNBins = 100;
  double ptMin = 0;
  double ptMax = 1000;
  double etaNBins = 50;
  double etaMin = -5;
  double etaMax = 5;
  //Total b, c and other jets
  myhistos2_["h2_BTaggingEff_Denom_b_"+jetrawtag]    = dirs_[dirs_.size() - 1].make<TH2D>("h2_BTaggingEff_Denom_b", ";p_{T} [GeV];#eta", ptNBins, ptMin, ptMax, etaNBins, etaMin, etaMax);
  myhistos2_["h2_BTaggingEff_Denom_c_"+jetrawtag]    = dirs_[dirs_.size() - 1].make<TH2D>("h2_BTaggingEff_Denom_c", ";p_{T} [GeV];#eta", ptNBins, ptMin, ptMax, etaNBins, etaMin, etaMax);
  myhistos2_["h2_BTaggingEff_Denom_udsg_"+jetrawtag] = dirs_[dirs_.size() - 1].make<TH2D>("h2_BTaggingEff_Denom_udsg", ";p_{T} [GeV];#eta", ptNBins, ptMin, ptMax, etaNBins, etaMin, etaMax);
  //Loose BTag efficiency 2D histos          
  myhistos2_["h2_BTaggingEff_Num_bL_"+jetrawtag]      = dirs_[dirs_.size() - 1].make<TH2D>("h2_BTaggingEff_Num_bL", ";p_{T} [GeV];#eta", ptNBins, ptMin, ptMax, etaNBins, etaMin, etaMax);
  myhistos2_["h2_BTaggingEff_Num_cL_"+jetrawtag]      = dirs_[dirs_.size() - 1].make<TH2D>("h2_BTaggingEff_Num_cL", ";p_{T} [GeV];#eta", ptNBins, ptMin, ptMax, etaNBins, etaMin, etaMax);
  myhistos2_["h2_BTaggingEff_Num_udsgL_"+jetrawtag]   = dirs_[dirs_.size() - 1].make<TH2D>("h2_BTaggingEff_Num_udsgL", ";p_{T} [GeV];#eta", ptNBins, ptMin, ptMax, etaNBins, etaMin, etaMax);
  //Medium BTag efficiency 2D histos          
  myhistos2_["h2_BTaggingEff_Num_bM_"+jetrawtag]      = dirs_[dirs_.size() - 1].make<TH2D>("h2_BTaggingEff_Num_bM", ";p_{T} [GeV];#eta", ptNBins, ptMin, ptMax, etaNBins, etaMin, etaMax);
  myhistos2_["h2_BTaggingEff_Num_cM_"+jetrawtag]      = dirs_[dirs_.size() - 1].make<TH2D>("h2_BTaggingEff_Num_cM", ";p_{T} [GeV];#eta", ptNBins, ptMin, ptMax, etaNBins, etaMin, etaMax);
  myhistos2_["h2_BTaggingEff_Num_udsgM_"+jetrawtag]   = dirs_[dirs_.size() - 1].make<TH2D>("h2_BTaggingEff_Num_udsgM", ";p_{T} [GeV];#eta", ptNBins, ptMin, ptMax, etaNBins, etaMin, etaMax);
  //Tight BTag efficiency 2D histos          
  myhistos2_["h2_BTaggingEff_Num_bT_"+jetrawtag]      = dirs_[dirs_.size() - 1].make<TH2D>("h2_BTaggingEff_Num_bT", ";p_{T} [GeV];#eta", ptNBins, ptMin, ptMax, etaNBins, etaMin, etaMax);
  myhistos2_["h2_BTaggingEff_Num_cT_"+jetrawtag]      = dirs_[dirs_.size() - 1].make<TH2D>("h2_BTaggingEff_Num_cT", ";p_{T} [GeV];#eta", ptNBins, ptMin, ptMax, etaNBins, etaMin, etaMax);
  myhistos2_["h2_BTaggingEff_Num_udsgT_"+jetrawtag]   = dirs_[dirs_.size() - 1].make<TH2D>("h2_BTaggingEff_Num_udsgT", ";p_{T} [GeV];#eta", ptNBins, ptMin, ptMax, etaNBins, etaMin, etaMax);
  
  ///Electrons
  TString elerawtag="Electrons";
  std::string eletag(elerawtag);
  //Make a new TDirectory
  dirs_.push_back( fs_->mkdir(eletag.c_str()) );
  myhistos_["pt_"+elerawtag] = dirs_[dirs_.size() - 1].make<TH1D>("pt_"+elerawtag, "Electron Pt", 200, 0., 500.);
  myhistos_["eta_"+elerawtag] = dirs_[dirs_.size() - 1].make<TH1D>("eta_"+elerawtag, "Electron #eta", 60, -5.0, 5.0);
  myhistos_["phi_"+elerawtag] = dirs_[dirs_.size() - 1].make<TH1D>("phi_"+elerawtag, "Electron #phi", 80, -4.05, 3.95);
  myhistos_["relCombPFIsoEA_"+elerawtag] = dirs_[dirs_.size() - 1].make<TH1D>("relCombPFIsoEA_"+elerawtag, "Electron rel pf iso", 1000, 0, 20.);
  myhistos_["cic_id_"+elerawtag] = dirs_[dirs_.size() - 1].make<TH1D>("cic_id_"+elerawtag, "Ele CiC id", 100, 0.,  100.);
  myhistos_["vbtf_id_"+elerawtag] = dirs_[dirs_.size() - 1].make<TH1D>("vbtf_id_"+elerawtag,"Ele VBTF id", 100,   0., 100.);

  ///Muons
  TString murawtag="Muons";
  std::string mutag(murawtag);
  //Make a new TDirectory
  dirs_.push_back( fs_->mkdir(mutag.c_str()) );	  
  myhistos_["pt_"+murawtag] = dirs_[dirs_.size() - 1].make<TH1D>("pt_"+murawtag, "Muon Pt", 200, 0., 500.);
  myhistos_["eta_"+murawtag] = dirs_[dirs_.size() - 1].make<TH1D>("eta_"+murawtag, "Muon #eta", 60, -5.0, 5.0);
  myhistos_["phi_"+murawtag] = dirs_[dirs_.size() - 1].make<TH1D>("phi_"+murawtag, "Muon #phi", 80, -4.05, 3.95);
  
  myhistos_["normChi2_"+murawtag] = dirs_[dirs_.size() - 1].make<TH1D>("normChi2_"+murawtag, "Muon track norm Chi2/dof", 200, 0, 100.);
  myhistos_["nHits_"+murawtag] = dirs_[dirs_.size() - 1].make<TH1D>("nHits_"+murawtag, "Muon track Hits", 500, 0, 500.);
  myhistos_["nMuonHits_"+murawtag] = dirs_[dirs_.size() - 1].make<TH1D>("nMuonHits_"+murawtag, "Muon track muon Hits", 500, 0, 500.);
  myhistos_["D0_"+murawtag] = dirs_[dirs_.size() - 1].make<TH1D>("D0_"+murawtag, "Muon D0", 100, 0, 500.);
  myhistos_["Dz_"+murawtag] = dirs_[dirs_.size() - 1].make<TH1D>("Dz_"+murawtag, "Muon Dz", 100, 0, 500.);
  myhistos_["pfRelIso_"+murawtag] = dirs_[dirs_.size() - 1].make<TH1D>("pfRelIso_"+murawtag, "Muon pf reliso", 100, 0, 50.); 

  ///METs
  TString metrawtag="METs";
  std::string mettag(metrawtag);
  //Make a new TDirectory
  dirs_.push_back( fs_->mkdir(mettag.c_str()) );
  myhistos_["pt_"+metrawtag] = dirs_[dirs_.size() - 1].make<TH1D>("met_"+metrawtag, "MET", 200, 0., 500.);
  myhistos_["sumet_"+metrawtag] = dirs_[dirs_.size() - 1].make<TH1D>("sumet_"+metrawtag, "sum Et", 3000, 0., 3000.);
  myhistos_["phi_"+metrawtag] = dirs_[dirs_.size() - 1].make<TH1D>("phi_"+metrawtag, "MET", 80, -4.05, 3.95);
}
