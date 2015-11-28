// -*- C++ -*-
//
// Package:    L1Trigger/L1TNtuples
// Class:      L1JetRecoTreeProducer
//
/**\class L1JetRecoTreeProducer L1JetRecoTreeProducer.cc L1Trigger/L1TNtuples/src/L1JetRecoTreeProducer.cc

 Description: Produces tree containing reco quantities


*/


// system include files
#include <memory>

// framework
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/Framework/interface/ESHandle.h"

// cond formats
#include "JetMETCorrections/JetCorrector/interface/JetCorrector.h"

// data formats
#include "DataFormats/JetReco/interface/CaloJetCollection.h"
#include "DataFormats/JetReco/interface/PFJetCollection.h"
#include "DataFormats/JetReco/interface/JetID.h"


// ROOT output stuff
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "TH1.h"
#include "TTree.h"
#include "TF1.h"

//local  data formats
#include "L1Trigger/L1TNtuples/interface/L1AnalysisRecoJetDataFormat.h"
#include "L1Trigger/L1TNtuples/interface/L1AnalysisRecoMetDataFormat.h"

//
// class declaration
//

class L1JetRecoTreeProducer : public edm::EDAnalyzer {
public:
  explicit L1JetRecoTreeProducer(const edm::ParameterSet&);
  ~L1JetRecoTreeProducer();


private:
  virtual void beginJob(void) ;
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
  virtual void endJob();

  void doPFJets(edm::Handle<reco::PFJetCollection> pfJets);
  void doCaloJets(edm::Handle<reco::CaloJetCollection> caloJets);

public:
  L1Analysis::L1AnalysisRecoJetDataFormat*              jet_data;

private:

  // output file
  edm::Service<TFileService> fs_;

  // tree
  TTree * tree_;

  // EDM input tags
  edm::EDGetTokenT<reco::PFJetCollection>     pfJetToken_;
  //  edm::EDGetTokenT<reco::CaloJetCollection>     caloJetToken_;
  //  edm::EDGetTokenT<edm::ValueMap<reco::JetID> > caloJetIdToken_;
  edm::EDGetTokenT<reco::JetCorrector>        jecToken_;

  // debug stuff
  bool pfJetsMissing_;
  double jetptThreshold_;
  unsigned int maxCl_;
  unsigned int maxJet_;
  unsigned int maxVtx_;
  unsigned int maxTrk_;
};



L1JetRecoTreeProducer::L1JetRecoTreeProducer(const edm::ParameterSet& iConfig):
  pfJetsMissing_(false)
{
  
  //  caloJetToken_ = consumes<reco::CaloJetCollection>(iConfig.getUntrackedParameter("caloJetToken",edm::InputTag("ak4CaloJets")));
  pfJetToken_ = consumes<reco::PFJetCollection>(iConfig.getUntrackedParameter("pfJetToken",edm::InputTag("ak4PFJetsCHS")));
  //  caloJetIdToken_ = consumes<edm::ValueMap<reco::JetID> >(iConfig.getUntrackedParameter("jetIdToken",edm::InputTag("ak4JetID")));
  jecToken_ = consumes<reco::JetCorrector>(iConfig.getUntrackedParameter<edm::InputTag>("jecToken"));

  jetptThreshold_ = iConfig.getParameter<double>      ("jetptThreshold");
  maxJet_         = iConfig.getParameter<unsigned int>("maxJet");

  jet_data = new L1Analysis::L1AnalysisRecoJetDataFormat();

  // set up output
  tree_=fs_->make<TTree>("JetRecoTree", "JetRecoTree");
  tree_->Branch("Jet", "L1Analysis::L1AnalysisRecoJetDataFormat", &jet_data, 32000, 3);

}


L1JetRecoTreeProducer::~L1JetRecoTreeProducer()
{

   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called to for each event  ------------
void L1JetRecoTreeProducer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{

  jet_data->Reset();

  // get jets
  edm::Handle<reco::PFJetCollection> pfJets;
  iEvent.getByToken(pfJetToken_, pfJets);

  // get jet ID
  //  edm::Handle<edm::ValueMap<reco::JetID> > jetsID;
  //iEvent.getByLabel(jetIdTag_,jetsID);

  //  edm::Handle<reco::JetCorrector> corrector;
  //  iEvent.getByToken(jecToken_, corrector);

  if (pfJets.isValid()) {

    jet_data->nJets=0;

    doPFJets(pfJets);

  }
  else {
    if (!pfJetsMissing_) {edm::LogWarning("MissingProduct") << "PFJets not found.  Branch will not be filled" << std::endl;}
    pfJetsMissing_ = true;
  }



  tree_->Fill();

}


void
L1JetRecoTreeProducer::doCaloJets(edm::Handle<reco::CaloJetCollection> caloJets) {


  for( auto it=caloJets->begin();
       it!=caloJets->end() && jet_data->nJets < maxJet_;
       ++it) {
    
    jet_data->et.push_back(it->et());
    jet_data->eta.push_back(it->eta());
    jet_data->phi.push_back(it->phi());
    jet_data->e.push_back(it->energy());
    jet_data->isPF.push_back(false);
    
    jet_data->eEMF.push_back(it->emEnergyFraction());
    jet_data->eEmEB.push_back(it->emEnergyInEB());
    jet_data->eEmEE.push_back(it->emEnergyInEE());
    jet_data->eEmHF.push_back(it->emEnergyInHF());
    jet_data->eHadHB.push_back(it->hadEnergyInHB());
    jet_data->eHadHE.push_back(it->hadEnergyInHE());
    jet_data->eHadHO.push_back(it->hadEnergyInHO());
    jet_data->eHadHF.push_back(it->hadEnergyInHF());
    jet_data->eMaxEcalTow.push_back(it->maxEInEmTowers());
    jet_data->eMaxHcalTow.push_back(it->maxEInHadTowers());
    jet_data->towerArea.push_back(it->towersArea());

    jet_data->nJets++;

  }


}


void
L1JetRecoTreeProducer::doPFJets(edm::Handle<reco::PFJetCollection> pfJets) {
  
  for( auto it=pfJets->begin();
       it!=pfJets->end() && jet_data->nJets < maxJet_;
       ++it) {
    
    jet_data->et.push_back(it->et());
    jet_data->eta.push_back(it->eta());
    jet_data->phi.push_back(it->phi());
    jet_data->e.push_back(it->energy());
    jet_data->isPF.push_back(true);
    
    jet_data->nJets++;
    
  }
  
}


// ------------ method called once each job just before starting event loop  ------------
void
L1JetRecoTreeProducer::beginJob(void)
{
}

// ------------ method called once each job just after ending the event loop  ------------
void
L1JetRecoTreeProducer::endJob() {
}

//define this as a plug-in
DEFINE_FWK_MODULE(L1JetRecoTreeProducer);
