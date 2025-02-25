// -- ntuple maker for Muon HLT study
// -- author: Kyeongpil Lee (Seoul National University, kplee@cern.ch)

#include "MuonHLTTool/MuonHLTNtupler/interface/MuonHLTSeedNtupler.h"

#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Common/interface/TriggerNames.h"
#include "FWCore/Common/interface/TriggerResultsByName.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/EDConsumerBase.h"

#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/Common/interface/View.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "DataFormats/HLTReco/interface/TriggerObject.h"
#include "DataFormats/L1Trigger/interface/Muon.h"
#include "DataFormats/Luminosity/interface/LumiDetails.h"
#include "DataFormats/Math/interface/deltaPhi.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonSelectors.h"
#include "DataFormats/MuonReco/interface/MuonTrackLinks.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/RecoCandidate/interface/IsoDeposit.h"
#include "DataFormats/RecoCandidate/interface/IsoDepositFwd.h"
#include "DataFormats/RecoCandidate/interface/RecoChargedCandidate.h"
#include "DataFormats/RecoCandidate/interface/RecoChargedCandidateFwd.h"
#include "DataFormats/RecoCandidate/interface/RecoChargedCandidateIsolation.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/Scalers/interface/LumiScalers.h"

#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"
#include "HLTrigger/HLTcore/interface/HLTEventAnalyzerAOD.h"
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"

#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"

#include "DataFormats/TrajectorySeed/interface/TrajectorySeed.h"
#include "DataFormats/TrajectorySeed/interface/TrajectorySeedCollection.h"
#include "DataFormats/TrajectorySeed/interface/PropagationDirection.h"
#include "DataFormats/TrajectoryState/interface/PTrajectoryStateOnDet.h"
#include "DataFormats/TrajectoryState/interface/LocalTrajectoryParameters.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticle.h"
#include "CommonTools/Utils/interface/associationMapFilterValues.h"

#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"
#include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h"


#include <map>
#include <string>
#include <iomanip>
#include "TTree.h"

using namespace std;
using namespace reco;
using namespace edm;


MuonHLTSeedNtupler::MuonHLTSeedNtupler(const edm::ParameterSet& iConfig):
trackerGeometryToken_(esConsumes<TrackerGeometry, TrackerDigiGeometryRecord>()),

// trackerHitAssociatorConfig_(iConfig, consumesCollector()),
associatorToken(consumes<reco::TrackToTrackingParticleAssociator>(iConfig.getUntrackedParameter<edm::InputTag>("associator"))),
seedAssociatorToken(consumes<reco::TrackToTrackingParticleAssociator>(iConfig.getUntrackedParameter<edm::InputTag>("seedAssociator"))),
trackingParticleToken(consumes<TrackingParticleCollection>(iConfig.getUntrackedParameter<edm::InputTag>("trackingParticle"))),

t_offlineVertex_     ( consumes< reco::VertexCollection >                 (iConfig.getUntrackedParameter<edm::InputTag>("offlineVertex"     )) ),
t_PUSummaryInfo_     ( consumes< std::vector<PileupSummaryInfo> >         (iConfig.getUntrackedParameter<edm::InputTag>("PUSummaryInfo"     )) ),

t_L1Muon_            ( consumes< l1t::MuonBxCollection  >                 (iConfig.getUntrackedParameter<edm::InputTag>("L1Muon"            )) ),
t_L2Muon_            ( consumes< reco::RecoChargedCandidateCollection >   (iConfig.getUntrackedParameter<edm::InputTag>("L2Muon"            )) ),

// t_L1TkMuon_          ( consumes< l1t::TkMuonCollection >                  (iConfig.getUntrackedParameter<edm::InputTag>("L1TkMuon"))),
// t_L1TkPrimaryVertex_ ( consumes< l1t::TkPrimaryVertexCollection >         (iConfig.getUntrackedParameter<edm::InputTag>("L1TkPrimaryVertex"))),

t_hltIterL3OISeedsFromL2Muons_ ( consumes< edm::View<TrajectorySeed> >     (iConfig.getUntrackedParameter<edm::InputTag>("hltIterL3OISeedsFromL2Muons")) ),
t_hltIter0IterL3MuonPixelSeedsFromPixelTracks_ ( consumes< edm::View<TrajectorySeed> >     (iConfig.getUntrackedParameter<edm::InputTag>("hltIter0IterL3MuonPixelSeedsFromPixelTracks")) ),
t_hltIter2IterL3MuonPixelSeeds_ ( consumes< edm::View<TrajectorySeed> >     (iConfig.getUntrackedParameter<edm::InputTag>("hltIter2IterL3MuonPixelSeeds")) ),
t_hltIter3IterL3MuonPixelSeeds_ ( consumes< edm::View<TrajectorySeed> >     (iConfig.getUntrackedParameter<edm::InputTag>("hltIter3IterL3MuonPixelSeeds")) ),
t_hltIter0IterL3FromL1MuonPixelSeedsFromPixelTracks_ ( consumes< edm::View<TrajectorySeed> >     (iConfig.getUntrackedParameter<edm::InputTag>("hltIter0IterL3FromL1MuonPixelSeedsFromPixelTracks")) ),
t_hltIter2IterL3FromL1MuonPixelSeeds_ ( consumes< edm::View<TrajectorySeed> >     (iConfig.getUntrackedParameter<edm::InputTag>("hltIter2IterL3FromL1MuonPixelSeeds")) ),
t_hltIter3IterL3FromL1MuonPixelSeeds_ ( consumes< edm::View<TrajectorySeed> >     (iConfig.getUntrackedParameter<edm::InputTag>("hltIter3IterL3FromL1MuonPixelSeeds")) ),

t_hltIterL3OIMuonTrack_    ( consumes< edm::View<reco::Track> >                  (iConfig.getUntrackedParameter<edm::InputTag>("hltIterL3OIMuonTrack"    )) ),
t_hltIter0IterL3MuonTrack_    ( consumes< edm::View<reco::Track> >               (iConfig.getUntrackedParameter<edm::InputTag>("hltIter0IterL3MuonTrack"    )) ),
t_hltIter2IterL3MuonTrack_    ( consumes< edm::View<reco::Track> >               (iConfig.getUntrackedParameter<edm::InputTag>("hltIter2IterL3MuonTrack"    )) ),
t_hltIter3IterL3MuonTrack_    ( consumes< edm::View<reco::Track> >               (iConfig.getUntrackedParameter<edm::InputTag>("hltIter3IterL3MuonTrack"    )) ),
t_hltIter0IterL3FromL1MuonTrack_    ( consumes< edm::View<reco::Track> >         (iConfig.getUntrackedParameter<edm::InputTag>("hltIter0IterL3FromL1MuonTrack"    )) ),
t_hltIter2IterL3FromL1MuonTrack_    ( consumes< edm::View<reco::Track> >         (iConfig.getUntrackedParameter<edm::InputTag>("hltIter2IterL3FromL1MuonTrack"    )) ),
t_hltIter3IterL3FromL1MuonTrack_    ( consumes< edm::View<reco::Track> >         (iConfig.getUntrackedParameter<edm::InputTag>("hltIter3IterL3FromL1MuonTrack"    )) ),

t_genParticle_       ( consumes< reco::GenParticleCollection >            (iConfig.getUntrackedParameter<edm::InputTag>("genParticle"       )) )
{
  mvaFileHltIter2IterL3MuonPixelSeeds_B_                      = iConfig.getParameter<edm::FileInPath>("mvaFileHltIter2IterL3MuonPixelSeeds_B");
  mvaFileHltIter2IterL3FromL1MuonPixelSeeds_B_                = iConfig.getParameter<edm::FileInPath>("mvaFileHltIter2IterL3FromL1MuonPixelSeeds_B");
  mvaFileHltIter2IterL3MuonPixelSeeds_E_                      = iConfig.getParameter<edm::FileInPath>("mvaFileHltIter2IterL3MuonPixelSeeds_E");
  mvaFileHltIter2IterL3FromL1MuonPixelSeeds_E_                = iConfig.getParameter<edm::FileInPath>("mvaFileHltIter2IterL3FromL1MuonPixelSeeds_E");

  mvaScaleMeanHltIter2IterL3MuonPixelSeeds_B_ =                      iConfig.getParameter<std::vector<double>>("mvaScaleMeanHltIter2IterL3MuonPixelSeeds_B");
  mvaScaleMeanHltIter2IterL3FromL1MuonPixelSeeds_B_ =                iConfig.getParameter<std::vector<double>>("mvaScaleMeanHltIter2IterL3FromL1MuonPixelSeeds_B");
  mvaScaleStdHltIter2IterL3MuonPixelSeeds_B_ =                       iConfig.getParameter<std::vector<double>>("mvaScaleStdHltIter2IterL3MuonPixelSeeds_B");
  mvaScaleStdHltIter2IterL3FromL1MuonPixelSeeds_B_ =                 iConfig.getParameter<std::vector<double>>("mvaScaleStdHltIter2IterL3FromL1MuonPixelSeeds_B");
  mvaScaleMeanHltIter2IterL3MuonPixelSeeds_E_ =                      iConfig.getParameter<std::vector<double>>("mvaScaleMeanHltIter2IterL3MuonPixelSeeds_E");
  mvaScaleMeanHltIter2IterL3FromL1MuonPixelSeeds_E_ =                iConfig.getParameter<std::vector<double>>("mvaScaleMeanHltIter2IterL3FromL1MuonPixelSeeds_E");
  mvaScaleStdHltIter2IterL3MuonPixelSeeds_E_ =                       iConfig.getParameter<std::vector<double>>("mvaScaleStdHltIter2IterL3MuonPixelSeeds_E");
  mvaScaleStdHltIter2IterL3FromL1MuonPixelSeeds_E_ =                 iConfig.getParameter<std::vector<double>>("mvaScaleStdHltIter2IterL3FromL1MuonPixelSeeds_E");

  mvaHltIter2IterL3MuonPixelSeeds_.push_back(
    std::make_pair( std::make_unique<SeedMvaEstimator>(mvaFileHltIter2IterL3MuonPixelSeeds_B_, mvaScaleMeanHltIter2IterL3MuonPixelSeeds_B_, mvaScaleStdHltIter2IterL3MuonPixelSeeds_B_, false, 7),
                    std::make_unique<SeedMvaEstimator>(mvaFileHltIter2IterL3MuonPixelSeeds_E_, mvaScaleMeanHltIter2IterL3MuonPixelSeeds_E_, mvaScaleStdHltIter2IterL3MuonPixelSeeds_E_, false, 7) )
  );
  mvaHltIter2IterL3FromL1MuonPixelSeeds_.push_back(
    std::make_pair( std::make_unique<SeedMvaEstimator>(mvaFileHltIter2IterL3FromL1MuonPixelSeeds_B_, mvaScaleMeanHltIter2IterL3FromL1MuonPixelSeeds_B_, mvaScaleStdHltIter2IterL3FromL1MuonPixelSeeds_B_, true, 7),
                    std::make_unique<SeedMvaEstimator>(mvaFileHltIter2IterL3FromL1MuonPixelSeeds_E_, mvaScaleMeanHltIter2IterL3FromL1MuonPixelSeeds_E_, mvaScaleStdHltIter2IterL3FromL1MuonPixelSeeds_E_, true, 7) )
  );
}

void MuonHLTSeedNtupler::analyze(const edm::Event &iEvent, const edm::EventSetup &iSetup)
{
  Init();

  // -- fill each object
  Fill_Event(iEvent);
  Fill_IterL3TT(iEvent);
  Fill_Seed(iEvent, iSetup);
  NTEvent_->Fill();
}

void MuonHLTSeedNtupler::beginJob()
{
  edm::Service<TFileService> fs;

  NTEvent_    = fs->make<TTree>("NTEvent","NTEvent");  

  NThltIterL3OI_    = fs->make<TTree>("NThltIterL3OI","NThltIterL3OI");

  NThltIter0_       = fs->make<TTree>("NThltIter0","NThltIter0");
  NThltIter2_       = fs->make<TTree>("NThltIter2","NThltIter2");
  NThltIter3_       = fs->make<TTree>("NThltIter3","NThltIter3");

  NThltIter0FromL1_ = fs->make<TTree>("NThltIter0FromL1","NThltIter0FromL1");
  NThltIter2FromL1_ = fs->make<TTree>("NThltIter2FromL1","NThltIter2FromL1");
  NThltIter3FromL1_ = fs->make<TTree>("NThltIter3FromL1","NThltIter3FromL1");

  Make_Branch();
}

void MuonHLTSeedNtupler::Init()
{
  runNum_       = -999;
  lumiBlockNum_ = -999;
  eventNum_     = 0;
  nVertex_      = -999;
  truePU_       = -999;
  nhltIterL3OI_ = 0;
  nhltIter0_ = 0;
  nhltIter2_ = 0;
  nhltIter3_ = 0;
  nhltIter0FromL1_ = 0;
  nhltIter2FromL1_ = 0;
  nhltIter3FromL1_ = 0;
  hltIterL3OIMuonTrackMap.clear();
  hltIter0IterL3MuonTrackMap.clear();
  hltIter2IterL3MuonTrackMap.clear();
  hltIter3IterL3MuonTrackMap.clear();
  hltIter0IterL3FromL1MuonTrackMap.clear();
  hltIter2IterL3FromL1MuonTrackMap.clear();
  hltIter3IterL3FromL1MuonTrackMap.clear();

  ST->clear();

  TThltIterL3OIMuonTrack->clear();
  TThltIter0IterL3MuonTrack->clear();
  TThltIter2IterL3MuonTrack->clear();
  TThltIter3IterL3MuonTrack->clear();
  TThltIter0IterL3FromL1MuonTrack->clear();
  TThltIter2IterL3FromL1MuonTrack->clear();
  TThltIter3IterL3FromL1MuonTrack->clear();
}

void MuonHLTSeedNtupler::Make_Branch()
{
  NTEvent_->Branch("runNum",&runNum_,"runNum/I");
  NTEvent_->Branch("lumiBlockNum",&lumiBlockNum_,"lumiBlockNum/I");
  NTEvent_->Branch("eventNum",&eventNum_,"eventNum/l"); // -- unsigned long long -- //
  NTEvent_->Branch("nVertex", &nVertex_, "nVertex/I");
  NTEvent_->Branch("truePU", &truePU_, "truePU/I");
  NTEvent_->Branch("nhltIterL3OI",  &nhltIterL3OI_, "nhltIterL3OI/I");
  NTEvent_->Branch("nhltIter0",  &nhltIter0_, "nhltIter0/I");
  NTEvent_->Branch("nhltIter2",  &nhltIter2_, "nhltIter2/I");
  NTEvent_->Branch("nhltIter3",  &nhltIter3_, "nhltIter3/I");
  NTEvent_->Branch("nhltIter0FromL1",  &nhltIter0FromL1_, "nhltIter0FromL1/I");
  NTEvent_->Branch("nhltIter2FromL1",  &nhltIter2FromL1_, "nhltIter2FromL1/I");
  NTEvent_->Branch("nhltIter3FromL1",  &nhltIter3FromL1_, "nhltIter3FromL1/I");
  ST->setBranch(NThltIterL3OI_);
  ST->setBranch(NThltIter0_);
  ST->setBranch(NThltIter2_);
  ST->setBranch(NThltIter3_);
  ST->setBranch(NThltIter0FromL1_);
  ST->setBranch(NThltIter2FromL1_);
  ST->setBranch(NThltIter3FromL1_);
}

void MuonHLTSeedNtupler::Fill_Event(const edm::Event &iEvent)
{
  // -- basic info.
  runNum_       = iEvent.id().run();
  lumiBlockNum_ = iEvent.id().luminosityBlock();
  eventNum_     = iEvent.id().event();

  // -- pile up
  edm::Handle<std::vector< PileupSummaryInfo > > h_PUSummaryInfo;
  if( iEvent.getByToken(t_PUSummaryInfo_,h_PUSummaryInfo) )
  {
    std::vector<PileupSummaryInfo>::const_iterator PVI;
    for(PVI = h_PUSummaryInfo->begin(); PVI != h_PUSummaryInfo->end(); ++PVI)
    {
      if(PVI->getBunchCrossing()==0)
      {
        truePU_ = PVI->getTrueNumInteractions();
        continue;
      }
    } // -- end of PU iteration -- //
  } // -- end of if ( token exists )

  // -- vertex
  edm::Handle<reco::VertexCollection> h_offlineVertex;
  if( iEvent.getByToken(t_offlineVertex_, h_offlineVertex) )
  {
    int nGoodVtx = 0;
    for(reco::VertexCollection::const_iterator it = h_offlineVertex->begin(); it != h_offlineVertex->end(); ++it)
      if( it->isValid() ) nGoodVtx++;

    nVertex_ = nGoodVtx;
  }
}

void MuonHLTSeedNtupler::Fill_IterL3TT(const edm::Event &iEvent)
{
  edm::Handle<reco::TrackToTrackingParticleAssociator> theAssociator;
  edm::Handle<TrackingParticleCollection> TPCollection;

  if( iEvent.getByToken(associatorToken, theAssociator) && iEvent.getByToken(trackingParticleToken, TPCollection) ) {
    fill_trackTemplate(iEvent,t_hltIterL3OIMuonTrack_,theAssociator,TPCollection,hltIterL3OIMuonTrackMap,TThltIterL3OIMuonTrack);
    fill_trackTemplate(iEvent,t_hltIter0IterL3MuonTrack_,theAssociator,TPCollection,hltIter0IterL3MuonTrackMap,TThltIter0IterL3MuonTrack);
    fill_trackTemplate(iEvent,t_hltIter2IterL3MuonTrack_,theAssociator,TPCollection,hltIter2IterL3MuonTrackMap,TThltIter2IterL3MuonTrack);
    fill_trackTemplate(iEvent,t_hltIter3IterL3MuonTrack_,theAssociator,TPCollection,hltIter3IterL3MuonTrackMap,TThltIter3IterL3MuonTrack);
    fill_trackTemplate(iEvent,t_hltIter0IterL3FromL1MuonTrack_,theAssociator,TPCollection,hltIter0IterL3FromL1MuonTrackMap,TThltIter0IterL3FromL1MuonTrack);
    fill_trackTemplate(iEvent,t_hltIter2IterL3FromL1MuonTrack_,theAssociator,TPCollection,hltIter2IterL3FromL1MuonTrackMap,TThltIter2IterL3FromL1MuonTrack);
    fill_trackTemplate(iEvent,t_hltIter3IterL3FromL1MuonTrack_,theAssociator,TPCollection,hltIter3IterL3FromL1MuonTrackMap,TThltIter3IterL3FromL1MuonTrack);
  }
}

void MuonHLTSeedNtupler::Fill_Seed(const edm::Event &iEvent, const edm::EventSetup &iSetup)
{
  // TrackerHitAssociator associate(iEvent, trackerHitAssociatorConfig_);
  // edm::ESHandle<TrackerGeometry> tracker;
  // iSetup.get<TrackerDigiGeometryRecord>().get(tracker);
  const TrackerGeometry& tracker = iSetup.getData(trackerGeometryToken_);

  fill_seedTemplate(iEvent, t_hltIterL3OISeedsFromL2Muons_,                       tracker, hltIterL3OIMuonTrackMap,          TThltIterL3OIMuonTrack,          NThltIterL3OI_,    nhltIterL3OI_ );
  fill_seedTemplate(iEvent, t_hltIter0IterL3MuonPixelSeedsFromPixelTracks_,       tracker, hltIter0IterL3MuonTrackMap,       TThltIter0IterL3MuonTrack,       NThltIter0_,       nhltIter0_ );
  fill_seedTemplate(iEvent, t_hltIter3IterL3MuonPixelSeeds_,                      tracker, hltIter3IterL3MuonTrackMap,       TThltIter3IterL3MuonTrack,       NThltIter3_,       nhltIter3_ );
  fill_seedTemplate(iEvent, t_hltIter0IterL3FromL1MuonPixelSeedsFromPixelTracks_, tracker, hltIter0IterL3FromL1MuonTrackMap, TThltIter0IterL3FromL1MuonTrack, NThltIter0FromL1_, nhltIter0FromL1_ );
  fill_seedTemplate(iEvent, t_hltIter3IterL3FromL1MuonPixelSeeds_,                tracker, hltIter3IterL3FromL1MuonTrackMap, TThltIter3IterL3FromL1MuonTrack, NThltIter3FromL1_, nhltIter3FromL1_ );

  fill_seedTemplate(iEvent, t_hltIter2IterL3MuonPixelSeeds_,                      mvaHltIter2IterL3MuonPixelSeeds_,                      tracker, hltIter2IterL3MuonTrackMap,       TThltIter2IterL3MuonTrack,       NThltIter2_,       nhltIter2_ );
  fill_seedTemplate(iEvent, t_hltIter2IterL3FromL1MuonPixelSeeds_,                mvaHltIter2IterL3FromL1MuonPixelSeeds_,                tracker, hltIter2IterL3FromL1MuonTrackMap, TThltIter2IterL3FromL1MuonTrack, NThltIter2FromL1_, nhltIter2FromL1_ );
}

void MuonHLTSeedNtupler::fill_trackTemplate(const edm::Event &iEvent, edm::EDGetTokenT<edm::View<reco::Track>>& theToken,
  edm::Handle<reco::TrackToTrackingParticleAssociator>& theAssociator_, edm::Handle<TrackingParticleCollection>& TPCollection_,
  std::map<tmpTSOD,unsigned int>& trkMap, trkTemplate* TTtrack) {

  edm::Handle<edm::View<reco::Track>> trkHandle;
  if( iEvent.getByToken( theToken, trkHandle ) )
  {
    auto recSimColl = theAssociator_->associateRecoToSim(trkHandle,TPCollection_);
    //cout<<"recSimColl.size() == "<<recSimColl.size()<<endl;
    //cout<<"trkHandle.size() == "<<trkHandle->size()<<endl;
    //cout<<"TPCollection_.size() == "<<TPCollection_->size()<<endl;

    for( unsigned int i = 0; i < trkHandle->size(); i++ )
    {
      TTtrack->fill(trkHandle->at(i));

      int linkNo = -1;
      TTtrack->linkIterL3(linkNo);

      const PTrajectoryStateOnDet tmpseed = trkHandle->at(i).seedRef()->startingState();
      tmpTSOD tsod(tmpseed);
      trkMap.insert(make_pair(tsod,i));

      auto track = trkHandle->refAt(i);
      auto TPfound = recSimColl.find(track);
      if (TPfound != recSimColl.end()) {
        const auto& TPmatch = TPfound->val;
	//cout << "TP found: " << TPmatch[0].first->pdgId() << endl;
	//cout<<"TPfound -> val[0] first :"<<TPmatch[0].first<<endl;
	//cout<<"TPfound -> val[0] second :"<<TPmatch[0].second<<endl;
        TTtrack->fillBestTP(TPmatch[0].first);
        TTtrack->fillBestTPsharedFrac(TPmatch[0].second);
        TTtrack->fillmatchedTPsize(TPmatch.size());
      } else {
	//cout << "TP not found: trk pt = " << track->pt() << endl;
        TTtrack->fillDummyTP();
        TTtrack->fillBestTPsharedFrac(-99999.);
        TTtrack->fillmatchedTPsize(0);
      }
    }
  }
}

void MuonHLTSeedNtupler::fill_seedTemplate(
  const edm::Event &iEvent,
  edm::EDGetTokenT<edm::View<TrajectorySeed>>& theToken,
  const TrackerGeometry& tracker,
  std::map<tmpTSOD,unsigned int>& trkMap,
  trkTemplate* TTtrack,
  TTree* NT,
  int &nSeed
) {

  edm::Handle<reco::TrackToTrackingParticleAssociator> theAssociator;
  edm::Handle<TrackingParticleCollection> theTPCollection;
  bool hasAsso = iEvent.getByToken(seedAssociatorToken, theAssociator) && iEvent.getByToken(trackingParticleToken, theTPCollection);

  edm::Handle<reco::GenParticleCollection> h_genParticle;
  bool hasGen = iEvent.getByToken(t_genParticle_, h_genParticle);

  edm::Handle<l1t::MuonBxCollection> h_L1Muon;
  bool hasL1 = iEvent.getByToken(t_L1Muon_, h_L1Muon);

  edm::Handle<reco::RecoChargedCandidateCollection> h_L2Muon;
  bool hasL2 = iEvent.getByToken( t_L2Muon_, h_L2Muon );

  edm::Handle< edm::View<TrajectorySeed> > seedHandle;
  if( iEvent.getByToken( theToken, seedHandle) )
  {
    nSeed = seedHandle->size();
    for( auto i=0U; i<seedHandle->size(); ++i )
    {
      const auto& seed(seedHandle->at(i));

      tmpTSOD seedTsod(seed.startingState());
      ST->clear();

      ST->fill_PU(
        truePU_
      );

      // -- Track association
      ST->fill(seed, tracker);
      std::map<tmpTSOD,unsigned int>::const_iterator where = trkMap.find(seedTsod);
      int idxtmpL3 = (where==trkMap.end()) ? -1 : trkMap[seedTsod];
      ST->fill_TP(TTtrack, idxtmpL3 );

      if( hasAsso )
      {
        auto recSimColl = theAssociator->associateRecoToSim(seedHandle,theTPCollection);
        auto seed = seedHandle->refAt(i);
        auto TPfound = recSimColl.find(seed);
        if (TPfound != recSimColl.end()) {
          const auto& TPmatch = TPfound->val;
          ST->fill_SeedTP(TPmatch[0].first);
          ST->fill_SeedTPsharedFrac(TPmatch[0].second);
          ST->fill_matchedSeedTPsize(TPmatch.size());
        }
      }

      GlobalVector global_p = tracker.idToDet(seed.startingState().detId())->surface().toGlobal(seed.startingState().parameters().momentum());
      GlobalPoint  global_x = tracker.idToDet(seed.startingState().detId())->surface().toGlobal(seed.startingState().parameters().position());

      // -- GenParticle (muon) tag -- //
      if( hasGen )
      {
        float gen_pt = -99999.;
        float gen_eta = -99999.;
        float gen_phi = -99999.;
        float dR_GenSeed = 99999.;
        for(auto genp = h_genParticle->begin(); genp != h_genParticle->end(); genp++)
        {
          if( fabs(genp->pdgId()) ==  13 && genp->status()==1 )
          {
            GlobalVector vec_seed_vtx( global_x.x() - genp->vx(), global_x.y() - genp->vy(), global_x.z() - genp->vz() );
            if( reco::deltaR( *genp, vec_seed_vtx ) < dR_GenSeed ) {
              dR_GenSeed = reco::deltaR( *genp, vec_seed_vtx );
              gen_pt = genp->pt();
              gen_eta = genp->eta();
              gen_phi = genp->phi();
            }
          }
        }
        ST->fill_Genvars(
          gen_pt,
          gen_eta,
          gen_phi
        );
      }

      // -- L1, L2 association
      if( hasL1 ) {
        int nL1Muon = 0;
        float dR_minDRL1SeedP = 99999.;
        float dPhi_minDRL1SeedP = 99999.;
        float dR_minDPhiL1SeedX = 99999.;
        float dPhi_minDPhiL1SeedX = 99999.;
        float dR_minDRL1SeedP_AtVtx = 99999.;
        float dPhi_minDRL1SeedP_AtVtx = 99999.;
        float dR_minDPhiL1SeedX_AtVtx = 99999.;
        float dPhi_minDPhiL1SeedX_AtVtx = 99999.;
        float L1Muon_pt = 99999.;
        float L1Muon_eta = 99999.;
        float L1Muon_phi = 99999.;
        for(int ibx = h_L1Muon->getFirstBX(); ibx<=h_L1Muon->getLastBX(); ++ibx)
        {
          if(ibx != 0) continue; // -- only take when ibx == 0 -- //
          for(auto it=h_L1Muon->begin(ibx); it!=h_L1Muon->end(ibx); it++)
          {
            l1t::MuonRef ref_L1Mu(h_L1Muon, distance(h_L1Muon->begin(h_L1Muon->getFirstBX()), it) );

            if(ref_L1Mu->hwQual() < 7)
              continue;

            nL1Muon ++;
            float dR_L1SeedP   = reco::deltaR( *ref_L1Mu, global_p);
            float dPhi_L1SeedP = reco::deltaPhi( ref_L1Mu->phi(), global_p.phi());
            float dR_L1SeedX   = reco::deltaR( *ref_L1Mu, global_x);
            float dPhi_L1SeedX = reco::deltaPhi( ref_L1Mu->phi(), global_x.phi());

            if( dR_L1SeedP < dR_minDRL1SeedP ) {
              dR_minDRL1SeedP = dR_L1SeedP;
              dPhi_minDRL1SeedP = dPhi_L1SeedP;
            }
            if( fabs(dPhi_L1SeedX) < fabs(dPhi_minDPhiL1SeedX) ) {
              dR_minDPhiL1SeedX = dR_L1SeedX;
              dPhi_minDPhiL1SeedX = dPhi_L1SeedX;
            }

            float dR_L1SeedP_AtVtx   = reco::deltaR( ref_L1Mu->etaAtVtx(), ref_L1Mu->phiAtVtx(), global_p.eta(), global_p.phi());
            float dPhi_L1SeedP_AtVtx = reco::deltaPhi( ref_L1Mu->phiAtVtx(), global_p.phi());
            float dR_L1SeedX_AtVtx   = reco::deltaR( ref_L1Mu->etaAtVtx(), ref_L1Mu->phiAtVtx(), global_x.eta(), global_x.phi());
            float dPhi_L1SeedX_AtVtx = reco::deltaPhi( ref_L1Mu->phiAtVtx(), global_x.phi());

            if( dR_L1SeedP_AtVtx < dR_minDRL1SeedP_AtVtx ) {
              dR_minDRL1SeedP_AtVtx = dR_L1SeedP_AtVtx;
              dPhi_minDRL1SeedP_AtVtx = dPhi_L1SeedP_AtVtx;
              L1Muon_pt = ref_L1Mu->pt();
              L1Muon_eta = ref_L1Mu->etaAtVtx();
              L1Muon_phi = ref_L1Mu->phiAtVtx();
            }
            if( fabs(dPhi_L1SeedX_AtVtx) < fabs(dPhi_minDPhiL1SeedX_AtVtx) ) {
              dR_minDPhiL1SeedX_AtVtx = dR_L1SeedX_AtVtx;
              dPhi_minDPhiL1SeedX_AtVtx = dPhi_L1SeedX_AtVtx;
            }
          }
        }
        ST->fill_L1vars(nL1Muon,
          dR_minDRL1SeedP,         dPhi_minDRL1SeedP,
          dR_minDPhiL1SeedX,       dPhi_minDPhiL1SeedX,
          dR_minDRL1SeedP_AtVtx,   dPhi_minDRL1SeedP_AtVtx,
          dR_minDPhiL1SeedX_AtVtx, dPhi_minDPhiL1SeedX_AtVtx,
          L1Muon_pt,               L1Muon_eta,                L1Muon_phi
        );
      }

      if( hasL2 && h_L2Muon->size() > 0 ) {
        int nL2Muon = 0;
        float dR_minDRL2SeedP = 99999.;
        float dPhi_minDRL2SeedP = 99999.;
        float dR_minDPhiL2SeedX = 99999.;
        float dPhi_minDPhiL2SeedX = 99999.;
        float L2Muon_pt = 99999.;
        float L2Muon_eta = 99999.;
        float L2Muon_phi = 99999.;
        nL2Muon = h_L2Muon->size();
        for( unsigned int i_L2=0; i_L2<h_L2Muon->size(); i_L2++)
        {
          reco::RecoChargedCandidateRef ref_L2Mu(h_L2Muon, i_L2);

          float dR_L2SeedP   = reco::deltaR( *ref_L2Mu, global_p);
          float dPhi_L2SeedP = reco::deltaPhi( ref_L2Mu->phi(), global_p.phi());
          float dR_L2SeedX   = reco::deltaR( *ref_L2Mu, global_x);
          float dPhi_L2SeedX = reco::deltaPhi( ref_L2Mu->phi(), global_x.phi());

          if( dR_L2SeedP < dR_minDRL2SeedP ) {
            dR_minDRL2SeedP = dR_L2SeedP;
            dPhi_minDRL2SeedP = dPhi_L2SeedP;
            L2Muon_pt = ref_L2Mu->pt();
            L2Muon_eta = ref_L2Mu->eta();
            L2Muon_phi = ref_L2Mu->phi();
          }
          if( fabs(dPhi_L2SeedX) < fabs(dPhi_minDPhiL2SeedX) ) {
            dR_minDPhiL2SeedX = dR_L2SeedX;
            dPhi_minDPhiL2SeedX = dPhi_L2SeedX;
          }
        }

        ST->fill_L2vars(nL2Muon,
          dR_minDRL2SeedP,         dPhi_minDRL2SeedP,
          dR_minDPhiL2SeedX,       dPhi_minDPhiL2SeedX,
          L2Muon_pt,               L2Muon_eta,                L2Muon_phi
        );
      }

      ST->fill_ntuple(NT);
    } // -- end of seed iteration
  } // -- if getByToken is valid
}

void MuonHLTSeedNtupler::fill_seedTemplate(
  const edm::Event &iEvent,
  edm::EDGetTokenT<edm::View<TrajectorySeed>>& theToken,
  const pairSeedMvaEstimator& pairMvaEstimator,
  const TrackerGeometry& tracker,
  std::map<tmpTSOD,unsigned int>& trkMap,
  trkTemplate* TTtrack,
  TTree* NT,
  int &nSeed
) {

  edm::Handle<reco::TrackToTrackingParticleAssociator> theAssociator;
  edm::Handle<TrackingParticleCollection> theTPCollection;
  bool hasAsso = iEvent.getByToken(seedAssociatorToken, theAssociator) && iEvent.getByToken(trackingParticleToken, theTPCollection);

  edm::Handle<reco::GenParticleCollection> h_genParticle;
  bool hasGen = iEvent.getByToken(t_genParticle_, h_genParticle);

  edm::Handle<l1t::MuonBxCollection> h_L1Muon;
  bool hasL1 = iEvent.getByToken(t_L1Muon_, h_L1Muon);
  const l1t::MuonBxCollection l1Muons = *(h_L1Muon.product());

  edm::Handle<reco::RecoChargedCandidateCollection> h_L2Muon;
  bool hasL2 = iEvent.getByToken( t_L2Muon_, h_L2Muon );
  const reco::RecoChargedCandidateCollection l2Muons = *(h_L2Muon.product());

  edm::Handle< edm::View<TrajectorySeed> > seedHandle;
  if( iEvent.getByToken( theToken, seedHandle) )
  {
    nSeed = seedHandle->size();
    for( auto i=0U; i<seedHandle->size(); ++i )
    {
      const auto& seed(seedHandle->at(i));

      tmpTSOD seedTsod(seed.startingState());
      ST->clear();

      ST->fill_PU(
        truePU_
      );

      // -- Track association
      ST->fill(seed, tracker);
      std::map<tmpTSOD,unsigned int>::const_iterator where = trkMap.find(seedTsod);
      int idxtmpL3 = (where==trkMap.end()) ? -1 : trkMap[seedTsod];
      //cout<<"[SeedNtupler] fill_TP : i="<<i<<", index tmpL3="<<idxtmpL3<<endl;
      ST->fill_TP(TTtrack, idxtmpL3 );

      if( hasAsso )
      {
        auto recSimColl = theAssociator->associateRecoToSim(seedHandle,theTPCollection);
        auto seed = seedHandle->refAt(i);
        auto TPfound = recSimColl.find(seed);
        if (TPfound != recSimColl.end()) {
          const auto& TPmatch = TPfound->val;
          ST->fill_SeedTP(TPmatch[0].first);
          ST->fill_SeedTPsharedFrac(TPmatch[0].second);
          ST->fill_matchedSeedTPsize(TPmatch.size());
        }
      }

      GlobalVector global_p = tracker.idToDet(seed.startingState().detId())->surface().toGlobal(seed.startingState().parameters().momentum());
      GlobalPoint  global_x = tracker.idToDet(seed.startingState().detId())->surface().toGlobal(seed.startingState().parameters().position());

      // -- BDT -- //
      vector<double> v_mva = {};
      for(auto ic=0U; ic<pairMvaEstimator.size(); ++ic) {
        if( fabs( global_p.eta() ) < 1.2 ) {
          double mva = pairMvaEstimator.at(ic).first->computeMva(
            seed,
            global_p,
            l1Muons,
            l2Muons
          );
          v_mva.push_back( mva );
        }
        else {
          double mva = pairMvaEstimator.at(ic).second->computeMva(
            seed,
            global_p,
            l1Muons,
            l2Muons
          );
          v_mva.push_back( mva );
        }
      }
      //if( v_mva.size() != 4 ) {
      //  cout << "v_mva.size() != 4" << endl;
      //  return;
      //}
      //ST->fill_Mva( v_mva[0], v_mva[1], v_mva[2], v_mva[3] );
      ST->fill_Mva( v_mva[0], -99999., -99999., -99999.);

      // -- GenParticle (muon) tag -- //
      if( hasGen )
      {
        float gen_pt = -99999.;
        float gen_eta = -99999.;
        float gen_phi = -99999.;
        float dR_GenSeed = 99999.;
        for(auto genp = h_genParticle->begin(); genp != h_genParticle->end(); genp++)
        {
          if( fabs(genp->pdgId()) ==  13 && genp->status()==1 )
          {
            GlobalVector vec_seed_vtx( global_x.x() - genp->vx(), global_x.y() - genp->vy(), global_x.z() - genp->vz() );
            if( reco::deltaR( *genp, vec_seed_vtx ) < dR_GenSeed ) {
              dR_GenSeed = reco::deltaR( *genp, vec_seed_vtx );
              gen_pt = genp->pt();
              gen_eta = genp->eta();
              gen_phi = genp->phi();
              //cout<<"(gen_pt, gen_eta, gen_phi, dR) = ("<<gen_pt<<", "<<gen_eta<<", "<<gen_phi<<", "<<dR_GenSeed<<")"<<endl;
            }
          }
        }
        ST->fill_Genvars(
          gen_pt,
          gen_eta,
          gen_phi
        );
      }

      // -- L1, L2 association
      if( hasL1 ) {
        int nL1Muon = 0;
        float dR_minDRL1SeedP = 99999.;
        float dPhi_minDRL1SeedP = 99999.;
        float dR_minDPhiL1SeedX = 99999.;
        float dPhi_minDPhiL1SeedX = 99999.;
        float dR_minDRL1SeedP_AtVtx = 99999.;
        float dPhi_minDRL1SeedP_AtVtx = 99999.;
        float dR_minDPhiL1SeedX_AtVtx = 99999.;
        float dPhi_minDPhiL1SeedX_AtVtx = 99999.;
        float L1Muon_pt = 99999.;
        float L1Muon_eta = 99999.;
        float L1Muon_phi = 99999.;
        for(int ibx = h_L1Muon->getFirstBX(); ibx<=h_L1Muon->getLastBX(); ++ibx)
        {
          if(ibx != 0) continue; // -- only take when ibx == 0 -- //
          for(auto it=h_L1Muon->begin(ibx); it!=h_L1Muon->end(ibx); it++)
          {
            l1t::MuonRef ref_L1Mu(h_L1Muon, distance(h_L1Muon->begin(h_L1Muon->getFirstBX()), it) );

            if(ref_L1Mu->hwQual() < 7)
              continue;

	    nL1Muon ++;
            float dR_L1SeedP   = reco::deltaR( *ref_L1Mu, global_p);
            float dPhi_L1SeedP = reco::deltaPhi( ref_L1Mu->phi(), global_p.phi());
            float dR_L1SeedX   = reco::deltaR( *ref_L1Mu, global_x);
            float dPhi_L1SeedX = reco::deltaPhi( ref_L1Mu->phi(), global_x.phi());

            if( dR_L1SeedP < dR_minDRL1SeedP ) {
              dR_minDRL1SeedP = dR_L1SeedP;
              dPhi_minDRL1SeedP = dPhi_L1SeedP;
            }
            if( fabs(dPhi_L1SeedX) < fabs(dPhi_minDPhiL1SeedX) ) {
              dR_minDPhiL1SeedX = dR_L1SeedX;
              dPhi_minDPhiL1SeedX = dPhi_L1SeedX;
            }

            float dR_L1SeedP_AtVtx   = reco::deltaR( ref_L1Mu->etaAtVtx(), ref_L1Mu->phiAtVtx(), global_p.eta(), global_p.phi());
            float dPhi_L1SeedP_AtVtx = reco::deltaPhi( ref_L1Mu->phiAtVtx(), global_p.phi());
            float dR_L1SeedX_AtVtx   = reco::deltaR( ref_L1Mu->etaAtVtx(), ref_L1Mu->phiAtVtx(), global_x.eta(), global_x.phi());
            float dPhi_L1SeedX_AtVtx = reco::deltaPhi( ref_L1Mu->phiAtVtx(), global_x.phi());

            if( dR_L1SeedP_AtVtx < dR_minDRL1SeedP_AtVtx ) {
              dR_minDRL1SeedP_AtVtx = dR_L1SeedP_AtVtx;
              dPhi_minDRL1SeedP_AtVtx = dPhi_L1SeedP_AtVtx;
              L1Muon_pt = ref_L1Mu->pt();
              L1Muon_eta = ref_L1Mu->etaAtVtx();
              L1Muon_phi = ref_L1Mu->phiAtVtx();
            }
            if( fabs(dPhi_L1SeedX_AtVtx) < fabs(dPhi_minDPhiL1SeedX_AtVtx) ) {
              dR_minDPhiL1SeedX_AtVtx = dR_L1SeedX_AtVtx;
              dPhi_minDPhiL1SeedX_AtVtx = dPhi_L1SeedX_AtVtx;
            }
          }
        }
        ST->fill_L1vars(nL1Muon,
          dR_minDRL1SeedP,         dPhi_minDRL1SeedP,
          dR_minDPhiL1SeedX,       dPhi_minDPhiL1SeedX,
          dR_minDRL1SeedP_AtVtx,   dPhi_minDRL1SeedP_AtVtx,
          dR_minDPhiL1SeedX_AtVtx, dPhi_minDPhiL1SeedX_AtVtx,
          L1Muon_pt,               L1Muon_eta,                L1Muon_phi
        );
      }

      if( hasL2 && h_L2Muon->size() > 0 ) {
        int nL2Muon = 0;
        float dR_minDRL2SeedP = 99999.;
        float dPhi_minDRL2SeedP = 99999.;
        float dR_minDPhiL2SeedX = 99999.;
        float dPhi_minDPhiL2SeedX = 99999.;
        float L2Muon_pt = 99999.;
        float L2Muon_eta = 99999.;
        float L2Muon_phi = 99999.;
        nL2Muon = h_L2Muon->size();
        for( unsigned int i_L2=0; i_L2<h_L2Muon->size(); i_L2++)
        {
          reco::RecoChargedCandidateRef ref_L2Mu(h_L2Muon, i_L2);

          float dR_L2SeedP   = reco::deltaR( *ref_L2Mu, global_p);
          float dPhi_L2SeedP = reco::deltaPhi( ref_L2Mu->phi(), global_p.phi());
          float dR_L2SeedX   = reco::deltaR( *ref_L2Mu, global_x);
          float dPhi_L2SeedX = reco::deltaPhi( ref_L2Mu->phi(), global_x.phi());

          if( dR_L2SeedP < dR_minDRL2SeedP ) {
            dR_minDRL2SeedP = dR_L2SeedP;
            dPhi_minDRL2SeedP = dPhi_L2SeedP;
            L2Muon_pt = ref_L2Mu->pt();
            L2Muon_eta = ref_L2Mu->eta();
            L2Muon_phi = ref_L2Mu->phi();
          }
          if( fabs(dPhi_L2SeedX) < fabs(dPhi_minDPhiL2SeedX) ) {
            dR_minDPhiL2SeedX = dR_L2SeedX;
            dPhi_minDPhiL2SeedX = dPhi_L2SeedX;
          }
        }

        ST->fill_L2vars(nL2Muon,
          dR_minDRL2SeedP,         dPhi_minDRL2SeedP,
          dR_minDPhiL2SeedX,       dPhi_minDPhiL2SeedX,
          L2Muon_pt,               L2Muon_eta,                L2Muon_phi
        );
      }

      ST->fill_ntuple(NT);
    } // -- end of seed iteration
  } // -- if getByToken is valid
}

void MuonHLTSeedNtupler::endJob() {

  //for( int i=0; i<4; ++i ) {
  // for( int i=0; i<1; ++i ) {
  //   delete mvaHltIter2IterL3MuonPixelSeeds_.at(i).first;
  //   delete mvaHltIter2IterL3MuonPixelSeeds_.at(i).second;
  //   delete mvaHltIter2IterL3FromL1MuonPixelSeeds_.at(i).first;
  //   delete mvaHltIter2IterL3FromL1MuonPixelSeeds_.at(i).second;
  // }

}
// void MuonHLTSeedNtupler::beginRun(const edm::Run &iRun, const edm::EventSetup &iSetup) {}
// void MuonHLTSeedNtupler::endRun(const edm::Run &iRun, const edm::EventSetup &iSetup) {}

DEFINE_FWK_MODULE(MuonHLTSeedNtupler);
