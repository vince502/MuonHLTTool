// -- ntuple maker for Muon HLT study
// -- author: Kyeongpil Lee (Seoul National University, kplee@cern.ch)

#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Common/interface/TriggerNames.h"
#include "FWCore/Common/interface/TriggerResultsByName.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"

#include "DataFormats/BeamSpot/interface/BeamSpot.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/Common/interface/Ref.h"
#include "DataFormats/Common/interface/View.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "DataFormats/HLTReco/interface/TriggerObject.h"
#include "DataFormats/L1Trigger/interface/Muon.h"
#include "DataFormats/Luminosity/interface/LumiDetails.h"
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
// #include "DataFormats/L1TrackTrigger/interface/TTTypes.h"
// #include "DataFormats/L1TrackTrigger/interface/TTCluster.h"
// #include "DataFormats/L1TrackTrigger/interface/TTStub.h"
// #include "DataFormats/L1TrackTrigger/interface/TTTrack.h"

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
#include "DataFormats/TrackingRecHit/interface/TrackingRecHit.h"
#include "DataFormats/HeavyIonEvent/interface/Centrality.h"

//--- for SimHit association
#include "SimDataFormats/TrackingHit/interface/PSimHit.h"
#include "SimTracker/TrackerHitAssociation/interface/TrackerHitAssociator.h"
#include "SimDataFormats/Associations/interface/TrackToTrackingParticleAssociator.h"
#include "SimTracker/Common/interface/TrackingParticleSelector.h"
#include "DataFormats/Common/interface/ValueMap.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticleFwd.h"

#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"
#include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h"
////////////////////////////
// DETECTOR GEOMETRY HEADERS
#include "MagneticField/Engine/interface/MagneticField.h"
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"
#include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h"
#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"
#include "Geometry/TrackerGeometryBuilder/interface/RectangularPixelTopology.h"
#include "Geometry/CommonDetUnit/interface/GeomDetType.h"
#include "Geometry/CommonDetUnit/interface/GeomDet.h"

// #include "Geometry/CommonTopologies/interface/PixelGeomDetUnit.h"
// #include "Geometry/CommonTopologies/interface/PixelGeomDetType.h"
#include "Geometry/TrackerGeometryBuilder/interface/PixelTopologyBuilder.h"
#include "Geometry/Records/interface/StackedTrackerGeometryRecord.h"

#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticle.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingVertex.h"
#include "SimDataFormats/TrackingHit/interface/PSimHitContainer.h"
#include "SimDataFormats/TrackingHit/interface/PSimHit.h"
// #include "SimTracker/TrackTriggerAssociation/interface/TTClusterAssociationMap.h"
// #include "SimTracker/TrackTriggerAssociation/interface/TTStubAssociationMap.h"
// #include "SimTracker/TrackTriggerAssociation/interface/TTTrackAssociationMap.h"
// #include "DataFormats/L1TCorrelator/interface/TkMuon.h"
// #include "DataFormats/L1TCorrelator/interface/TkMuonFwd.h"
// #include "DataFormats/L1TCorrelator/interface/TkPrimaryVertex.h"

#include "RecoMuon/TrackerSeedGenerator/interface/SeedMvaEstimator.h"
#include "MuonAnalysis/MuonAssociators/interface/PropagateToMuonSetup.h"

// #include "MuonHLTTool/MuonHLTNtupler/interface/MuonHLTobjCorrelator.h"

#include "TTree.h"
#include "TString.h"

using namespace std;
using namespace reco;
using namespace edm;

class MuonHLTNtupler : public edm::one::EDAnalyzer<>
{
public:
  explicit MuonHLTNtupler(const edm::ParameterSet &iConfig);
  virtual ~MuonHLTNtupler() {};

  virtual void analyze(const edm::Event &iEvent, const edm::EventSetup &iSetup);
  virtual void beginJob();
  virtual void endJob();

  // virtual void beginRun(const edm::Run &iRun, const edm::EventSetup &iSetup);
  // virtual void endRun(const edm::Run &iRun, const edm::EventSetup &iSetup);

private:
  void Init();
  void Make_Branch();
  // void Fill_L1Track(const edm::Event &iEvent, const edm::EventSetup &iSetup);
  void Fill_HLT(const edm::Event &iEvent, bool isMYHLT);
  void Fill_Muon(const edm::Event &iEvent, const edm::EventSetup &iSetup);
  void Fill_HLTMuon(const edm::Event &iEvent);
  void Fill_L1Muon(const edm::Event &iEvent);
  void Fill_GenParticle(const edm::Event &iEvent);

  //For Rerun (Fill_IterL3*)
  void Fill_IterL3(const edm::Event &iEvent, const edm::EventSetup &iSetup);
  void Fill_Seed(const edm::Event &iEvent, const edm::EventSetup &iSetup);

  bool SavedTriggerCondition( std::string& pathName );
  bool SavedFilterCondition( std::string& filterName );

  bool isNewHighPtMuon(const reco::Muon& muon, const reco::Vertex& vtx);

  bool doMVA;
  bool doHI;
  bool doSeed;
  bool DebugMode;
  // bool SaveAllTracks;   // store in ntuples not only truth-matched tracks but ALL tracks
  // bool SaveStubs;       // option to save also stubs in the ntuples (makes them large...)

  const PropagateToMuonSetup propSetup_;
  const edm::ESGetToken<TrackerGeometry, TrackerDigiGeometryRecord> trackerGeometryToken_;

  // edm::EDGetTokenT< std::vector< TTTrack< Ref_Phase2TrackerDigi_ > > > ttTrackToken_;
  // edm::EDGetTokenT< TTTrackAssociationMap< Ref_Phase2TrackerDigi_ > > ttTrackMCTruthToken_;
  // edm::EDGetTokenT< edmNew::DetSetVector< TTStub< Ref_Phase2TrackerDigi_ > > > ttStubToken_;
  // edm::EDGetTokenT<l1t::TkMuonCollection> TkMuonToken_;
  // edm::EDGetTokenT<l1t::TkPrimaryVertexCollection> l1TkPrimaryVertexToken_;

  // TrackerHitAssociator::Config trackerHitAssociatorConfig_;
  edm::EDGetTokenT<reco::TrackToTrackingParticleAssociator> associatorToken;
  edm::EDGetTokenT<TrackingParticleCollection> trackingParticleToken;



  edm::EDGetTokenT< reco::BeamSpot >                         t_beamSpot_;
  edm::EDGetTokenT< std::vector<reco::Muon> >                t_offlineMuon_;
  edm::EDGetTokenT< reco::VertexCollection >                 t_offlineVertex_;
  edm::EDGetTokenT< edm::TriggerResults >                    t_triggerResults_;
  edm::EDGetTokenT< trigger::TriggerEvent >                  t_triggerEvent_;
  edm::EDGetTokenT< edm::TriggerResults >                    t_myTriggerResults_;
  edm::EDGetTokenT< trigger::TriggerEvent >                  t_myTriggerEvent_;

  edm::EDGetTokenT< reco::RecoChargedCandidateCollection >   t_L3Muon_;
  edm::EDGetTokenT< double >                                 t_rho_ECAL_;
  edm::EDGetTokenT< double >                                 t_rho_HCAL_;
  edm::EDGetTokenT< reco::RecoChargedCandidateIsolationMap > t_ECALIsoMap_;
  edm::EDGetTokenT< reco::RecoChargedCandidateIsolationMap > t_HCALIsoMap_;
  edm::EDGetTokenT< reco::IsoDepositMap >                    t_trkIsoMap_;
  edm::EDGetTokenT< reco::RecoChargedCandidateCollection >   t_L2Muon_;
  edm::EDGetTokenT< l1t::MuonBxCollection >                  t_L1Muon_;
  edm::EDGetTokenT< reco::RecoChargedCandidateCollection >   t_TkMuon_;

  edm::EDGetTokenT< std::vector<reco::MuonTrackLinks> >      t_iterL3OI_;
  edm::EDGetTokenT< std::vector<reco::MuonTrackLinks> >      t_iterL3IOFromL2_;
  edm::EDGetTokenT< std::vector<reco::MuonTrackLinks> >      t_iterL3FromL2_;
  edm::EDGetTokenT< std::vector<reco::Track> >               t_iterL3IOFromL1_;
  edm::EDGetTokenT< std::vector<reco::Muon> >                t_iterL3MuonNoID_;
  edm::EDGetTokenT< std::vector<reco::Muon> >                t_iterL3Muon_;

  edm::EDGetTokenT< reco::VertexCollection >                 t_hltIterL3MuonTrimmedPixelVertices_;
  edm::EDGetTokenT< reco::VertexCollection >                 t_hltIterL3FromL1MuonTrimmedPixelVertices_;

  edm::EDGetTokenT< LumiScalersCollection >                  t_lumiScaler_;
  edm::EDGetTokenT< LumiScalersCollection >                  t_offlineLumiScaler_;
  edm::EDGetTokenT< std::vector<PileupSummaryInfo> >         t_PUSummaryInfo_;
  edm::EDGetTokenT< GenEventInfoProduct >                    t_genEventInfo_;
  edm::EDGetTokenT< edm::View<reco::GenParticle> >           t_genParticle_;

  std::vector<std::string>   trackCollectionNames_;
  std::vector<edm::InputTag> trackCollectionLabels_;
  std::vector<edm::InputTag> associationLabels_;
  std::vector<edm::EDGetTokenT< edm::View<reco::Track> > >  trackCollectionTokens_;
  std::vector<edm::EDGetTokenT<reco::SimToRecoCollection> > simToRecoCollectionTokens_;
  std::vector<edm::EDGetTokenT<reco::RecoToSimCollection> > recoToSimCollectionTokens_;

  edm::EDGetTokenT<pat::TriggerObjectStandAloneMatch>      t_recol1Matches_;
  edm::EDGetTokenT<edm::ValueMap<int>>                     t_recol1MatchesQuality_;
  edm::EDGetTokenT<edm::ValueMap<float>>                   t_recol1MatchesDeltaR_;
  edm::EDGetTokenT<pat::TriggerObjectStandAloneMatch>      t_recol1MatchesByQ_;
  edm::EDGetTokenT<edm::ValueMap<int>>                     t_recol1MatchesByQQuality_;
  edm::EDGetTokenT<edm::ValueMap<float>>                   t_recol1MatchesByQDeltaR_;
  edm::EDGetTokenT<pat::TriggerObjectStandAloneMatch>      t_genl1Matches_;
  edm::EDGetTokenT<edm::ValueMap<int>>                     t_genl1MatchesQuality_;
  edm::EDGetTokenT<edm::ValueMap<float>>                   t_genl1MatchesDeltaR_;
  edm::EDGetTokenT<pat::TriggerObjectStandAloneMatch>      t_genl1MatchesByQ_;
  edm::EDGetTokenT<edm::ValueMap<int>>                     t_genl1MatchesByQQuality_;
  edm::EDGetTokenT<edm::ValueMap<float>>                   t_genl1MatchesByQDeltaR_;
  edm::EDGetTokenT<reco::Centrality> CentralityTag_;
  edm::EDGetTokenT<int> CentralityBinTag_;

  // typedef std::vector< std::pair<SeedMvaEstimator*, SeedMvaEstimator*> > pairSeedMvaEstimator;
  typedef std::vector< std::pair<std::unique_ptr<const SeedMvaEstimator>, std::unique_ptr<const SeedMvaEstimator>>> pairSeedMvaEstimator;

  const reco::BeamSpot* bs;

  TTree *ntuple_;
  static const int arrSize_ = 5000;

  // -- general event information
  bool isRealData_;
  int runNum_;
  int lumiBlockNum_;
  unsigned long long eventNum_;

  double bs_x0_;
  double bs_y0_;
  double bs_z0_;
  double bs_sigmaZ_;
  double bs_dxdz_;
  double bs_dydz_;
  double bs_x0Error_;
  double bs_y0Error_;
  double bs_z0Error_;
  double bs_sigmaZ0Error_;
  double bs_dxdzError_;
  double bs_dydzError_;

  int nVertex_;

  double bunchID_;
  double instLumi_;
  double dataPU_;
  double dataPURMS_;
  double bunchLumi_;
  double offlineInstLumi_;
  double offlineDataPU_;
  double offlineDataPURMS_;
  double offlineBunchLumi_;
  int truePU_;
  double genEventWeight_;

  double rho_ECAL_;
  double rho_HCAL_;

  // -- generator level particles (only MC)
  int nGenParticle_;
  int genParticle_ID_[arrSize_];
  int genParticle_status_[arrSize_];
  int genParticle_mother_[arrSize_];

  double genParticle_pt_[arrSize_];
  double genParticle_eta_[arrSize_];
  double genParticle_phi_[arrSize_];
  double genParticle_px_[arrSize_];
  double genParticle_py_[arrSize_];
  double genParticle_pz_[arrSize_];
  double genParticle_energy_[arrSize_];
  double genParticle_charge_[arrSize_];

  int genParticle_isPrompt_[arrSize_];
  int genParticle_isPromptFinalState_[arrSize_];
  int genParticle_isTauDecayProduct_[arrSize_];
  int genParticle_isPromptTauDecayProduct_[arrSize_];
  int genParticle_isDirectPromptTauDecayProductFinalState_[arrSize_];
  int genParticle_isHardProcess_[arrSize_];
  int genParticle_isLastCopy_[arrSize_];
  int genParticle_isLastCopyBeforeFSR_[arrSize_];
  int genParticle_isPromptDecayed_[arrSize_];
  int genParticle_isDecayedLeptonHadron_[arrSize_];
  int genParticle_fromHardProcessBeforeFSR_[arrSize_];
  int genParticle_fromHardProcessDecayed_[arrSize_];
  int genParticle_fromHardProcessFinalState_[arrSize_];
  int genParticle_isMostlyLikePythia6Status3_[arrSize_];
  int hi_cBin;
  float hiHF;
  float hiHFplus;
  float hiHFminus;
  float hiHFeta4;
  float hiHFplusEta4;
  float hiHFminusEta4;
  float hiHFhit;
  float hiNpix;
  float hiNpixelTracks;
  float hiNtracks;
  float hiEB;
  float hiEE;
  float hiET;



  double genParticle_l1pt_[arrSize_];
  double genParticle_l1eta_[arrSize_];
  double genParticle_l1phi_[arrSize_];
  double genParticle_l1charge_[arrSize_];
  int    genParticle_l1q_[arrSize_];
  double genParticle_l1dr_[arrSize_];
  double genParticle_l1ptByQ_[arrSize_];
  double genParticle_l1etaByQ_[arrSize_];
  double genParticle_l1phiByQ_[arrSize_];
  double genParticle_l1chargeByQ_[arrSize_];
  int    genParticle_l1qByQ_[arrSize_];
  double genParticle_l1drByQ_[arrSize_];

  // -- trigger info.
  vector< std::string > vec_firedTrigger_;
  vector< std::string > vec_filterName_;
  vector< double > vec_HLTObj_pt_;
  vector< double > vec_HLTObj_eta_;
  vector< double > vec_HLTObj_phi_;

  vector< std::string > vec_myFiredTrigger_;
  vector< std::string > vec_myFilterName_;
  vector< double > vec_myHLTObj_pt_;
  vector< double > vec_myHLTObj_eta_;
  vector< double > vec_myHLTObj_phi_;

  // std::map<MuonHLTobjCorrelator::L1TTTrack,unsigned int> mTTTrackMap;

  class tmpTSOD {
  private:
    uint32_t TSODDetId;
    float TSODPt;
    float TSODX;
    float TSODY;
    float TSODDxdz;
    float TSODDydz;
    float TSODPx;
    float TSODPy;
    float TSODPz;
    float TSODqbp;
    int TSODCharge;
  public:
    void SetTmpTSOD(const PTrajectoryStateOnDet TSODIn) {
      TSODDetId = TSODIn.detId();
      TSODPt = TSODIn.pt();
      TSODX = TSODIn.parameters().position().x();
      TSODY = TSODIn.parameters().position().y();
      TSODDxdz = TSODIn.parameters().dxdz();
      TSODDydz = TSODIn.parameters().dydz();
      TSODPx = TSODIn.parameters().momentum().x();
      TSODPy = TSODIn.parameters().momentum().y();
      TSODPz = TSODIn.parameters().momentum().z();
      TSODqbp = TSODIn.parameters().qbp();
      TSODCharge = TSODIn.parameters().charge();
    }

    tmpTSOD(const PTrajectoryStateOnDet TSODIn) { SetTmpTSOD(TSODIn); }

    bool operator==(const tmpTSOD& other) const {
      return (
        this->TSODDetId == other.TSODDetId &&
        this->TSODPt == other.TSODPt &&
        this->TSODX == other.TSODX &&
        this->TSODY == other.TSODX &&
        this->TSODDxdz == other.TSODDxdz &&
        this->TSODDydz == other.TSODDydz &&
        this->TSODPx == other.TSODPx &&
        this->TSODPy == other.TSODPy &&
        this->TSODPz == other.TSODPz &&
        this->TSODqbp == other.TSODqbp &&
        this->TSODCharge == other.TSODCharge
      );
    }

    bool operator<(const tmpTSOD& other) const {
      return (this->TSODPt!=other.TSODPt) ? (this->TSODPt < other.TSODPt) : (this->TSODDetId < other.TSODDetId);
    }
  };

  // -- offline muon
  int nMuon_;

  double muon_pt_[arrSize_];
  double muon_eta_[arrSize_];
  double muon_phi_[arrSize_];
  double muon_px_[arrSize_];
  double muon_py_[arrSize_];
  double muon_pz_[arrSize_];
  double muon_dB_[arrSize_];
  double muon_charge_[arrSize_];
  int muon_isGLB_[arrSize_];
  int muon_isSTA_[arrSize_];
  int muon_isTRK_[arrSize_];
  int muon_isPF_[arrSize_];
  int muon_isTight_[arrSize_];
  int muon_isMedium_[arrSize_];
  int muon_isLoose_[arrSize_];
  int muon_isHighPt_[arrSize_];
  int muon_isHighPtNew_[arrSize_];
  int muon_isSoft_[arrSize_];

  double muon_iso03_sumPt_[arrSize_];
  double muon_iso03_hadEt_[arrSize_];
  double muon_iso03_emEt_[arrSize_];

  double muon_PFIso03_charged_[arrSize_];
  double muon_PFIso03_neutral_[arrSize_];
  double muon_PFIso03_photon_[arrSize_];
  double muon_PFIso03_sumPU_[arrSize_];

  double muon_PFIso04_charged_[arrSize_];
  double muon_PFIso04_neutral_[arrSize_];
  double muon_PFIso04_photon_[arrSize_];
  double muon_PFIso04_sumPU_[arrSize_];

  double muon_PFCluster03_ECAL_[arrSize_];
  double muon_PFCluster03_HCAL_[arrSize_];

  double muon_PFCluster04_ECAL_[arrSize_];
  double muon_PFCluster04_HCAL_[arrSize_];

  double muon_inner_trkChi2_[arrSize_];
  double muon_inner_validFraction_[arrSize_];
  int    muon_inner_trackerLayers_[arrSize_];
  int    muon_inner_trackerHits_[arrSize_];
  int    muon_inner_lostTrackerHits_[arrSize_];
  int    muon_inner_lostTrackerHitsIn_[arrSize_];
  int    muon_inner_lostTrackerHitsOut_[arrSize_];
  int    muon_inner_lostPixelHits_[arrSize_];
  int    muon_inner_lostPixelBarrelHits_[arrSize_];
  int    muon_inner_lostPixelEndcapHits_[arrSize_];
  int    muon_inner_lostStripHits_[arrSize_];
  int    muon_inner_lostStripTIBHits_[arrSize_];
  int    muon_inner_lostStripTIDHits_[arrSize_];
  int    muon_inner_lostStripTOBHits_[arrSize_];
  int    muon_inner_lostStripTECHits_[arrSize_];
  int    muon_inner_pixelLayers_[arrSize_];
  int    muon_inner_pixelHits_[arrSize_];
  int    muon_global_muonHits_[arrSize_];
  double muon_global_trkChi2_[arrSize_];
  int    muon_global_trackerLayers_[arrSize_];
  int    muon_global_trackerHits_[arrSize_];
  double muon_momentumChi2_[arrSize_];
  double muon_positionChi2_[arrSize_];
  double muon_glbKink_[arrSize_];
  double muon_glbTrackProbability_[arrSize_];
  double muon_globalDeltaEtaPhi_[arrSize_];
  double muon_localDistance_[arrSize_];
  double muon_staRelChi2_[arrSize_];
  int    muon_tightMatch_[arrSize_];
  double muon_trkKink_[arrSize_];
  double muon_trkRelChi2_[arrSize_];
  double muon_segmentCompatibility_[arrSize_];

  double muon_pt_tuneP_[arrSize_];
  double muon_ptError_tuneP_[arrSize_];

  double muon_dxyVTX_best_[arrSize_];
  double muon_dzVTX_best_[arrSize_];

  int muon_nMatchedStation_[arrSize_];
  int muon_nMatchedRPCLayer_[arrSize_];
  int muon_stationMask_[arrSize_];

  double muon_dxy_bs_[arrSize_];
  double muon_dxyError_bs_[arrSize_];
  double muon_dz_bs_[arrSize_];
  double muon_dzError_[arrSize_];
  double muon_IPSig_[arrSize_];

  double muon_l1pt_[arrSize_];
  double muon_l1eta_[arrSize_];
  double muon_l1phi_[arrSize_];
  double muon_l1charge_[arrSize_];
  int    muon_l1q_[arrSize_];
  double muon_l1dr_[arrSize_];
  double muon_l1ptByQ_[arrSize_];
  double muon_l1etaByQ_[arrSize_];
  double muon_l1phiByQ_[arrSize_];
  double muon_l1chargeByQ_[arrSize_];
  int    muon_l1qByQ_[arrSize_];
  double muon_l1drByQ_[arrSize_];

  int muon_nl1t_[arrSize_];
  vector<vector<double>> muon_l1tpt_;
  vector<vector<double>> muon_l1teta_;
  vector<vector<double>> muon_l1tpropeta_;
  vector<vector<double>> muon_l1tphi_;
  vector<vector<double>> muon_l1tpropphi_;
  vector<vector<double>> muon_l1tcharge_;
  vector<vector<double>> muon_l1tq_;
  vector<vector<double>> muon_l1tdr_;

  std::map<tmpTSOD,unsigned int> MuonIterSeedMap;
  std::map<tmpTSOD,unsigned int> MuonIterNoIdSeedMap;
  std::map<tmpTSOD,unsigned int> hltIterL3OIMuonTrackMap;
  std::map<tmpTSOD,unsigned int> hltIter0IterL3MuonTrackMap;
  std::map<tmpTSOD,unsigned int> hltIter2IterL3MuonTrackMap;
  std::map<tmpTSOD,unsigned int> hltIter3IterL3MuonTrackMap;
  std::map<tmpTSOD,unsigned int> hltIter0IterL3FromL1MuonTrackMap;
  std::map<tmpTSOD,unsigned int> hltIter2IterL3FromL1MuonTrackMap;
  std::map<tmpTSOD,unsigned int> hltIter3IterL3FromL1MuonTrackMap;

  // -- L3 muon
  int nL3Muon_;
  double L3Muon_pt_[arrSize_];
  double L3Muon_eta_[arrSize_];
  double L3Muon_phi_[arrSize_];
  double L3Muon_charge_[arrSize_];
  double L3Muon_trkPt_[arrSize_];

  double L3Muon_ECALIso_[arrSize_];
  double L3Muon_HCALIso_[arrSize_];
  double L3Muon_trkIso_[arrSize_];

  // -- L2 muon
  int nL2Muon_;
  double L2Muon_pt_[arrSize_];
  double L2Muon_eta_[arrSize_];
  double L2Muon_phi_[arrSize_];
  double L2Muon_charge_[arrSize_];
  double L2Muon_trkPt_[arrSize_];

  // -- L1 muon
  int nL1Muon_;
  double L1Muon_pt_[arrSize_];
  double L1Muon_eta_[arrSize_];
  double L1Muon_phi_[arrSize_];
  double L1Muon_charge_[arrSize_];
  double L1Muon_quality_[arrSize_];
  double L1Muon_etaAtVtx_[arrSize_];
  double L1Muon_phiAtVtx_[arrSize_];

  // -- Tracker muon
  int nTkMuon_;
  double TkMuon_pt_[arrSize_];
  double TkMuon_eta_[arrSize_];
  double TkMuon_phi_[arrSize_];
  double TkMuon_charge_[arrSize_];
  double TkMuon_trkPt_[arrSize_];

  // -- iterL3 object from outside-in
  int    nIterL3OI_;
  double iterL3OI_inner_pt_[arrSize_];
  double iterL3OI_inner_eta_[arrSize_];
  double iterL3OI_inner_phi_[arrSize_];
  double iterL3OI_inner_charge_[arrSize_];
  double iterL3OI_inner_trkChi2_[arrSize_];
  double iterL3OI_inner_validFraction_[arrSize_];
  int    iterL3OI_inner_trackerLayers_[arrSize_];
  int    iterL3OI_inner_trackerHits_[arrSize_];
  int    iterL3OI_inner_lostTrackerHits_[arrSize_];
  int    iterL3OI_inner_lostTrackerHitsIn_[arrSize_];
  int    iterL3OI_inner_lostTrackerHitsOut_[arrSize_];
  int    iterL3OI_inner_lostPixelHits_[arrSize_];
  int    iterL3OI_inner_lostPixelBarrelHits_[arrSize_];
  int    iterL3OI_inner_lostPixelEndcapHits_[arrSize_];
  int    iterL3OI_inner_lostStripHits_[arrSize_];
  int    iterL3OI_inner_lostStripTIBHits_[arrSize_];
  int    iterL3OI_inner_lostStripTIDHits_[arrSize_];
  int    iterL3OI_inner_lostStripTOBHits_[arrSize_];
  int    iterL3OI_inner_lostStripTECHits_[arrSize_];
  int    iterL3OI_inner_pixelLayers_[arrSize_];
  int    iterL3OI_inner_pixelHits_[arrSize_];
  double iterL3OI_outer_pt_[arrSize_];
  double iterL3OI_outer_eta_[arrSize_];
  double iterL3OI_outer_phi_[arrSize_];
  double iterL3OI_outer_charge_[arrSize_];
  double iterL3OI_global_pt_[arrSize_];
  double iterL3OI_global_eta_[arrSize_];
  double iterL3OI_global_phi_[arrSize_];
  double iterL3OI_global_charge_[arrSize_];
  int    iterL3OI_global_muonHits_[arrSize_];
  double iterL3OI_global_trkChi2_[arrSize_];
  int    iterL3OI_global_trackerLayers_[arrSize_];
  int    iterL3OI_global_trackerHits_[arrSize_];

  // -- iterL3 object from inside-out step (from L2)
  int    nIterL3IOFromL2_;
  double iterL3IOFromL2_inner_pt_[arrSize_];
  double iterL3IOFromL2_inner_eta_[arrSize_];
  double iterL3IOFromL2_inner_phi_[arrSize_];
  double iterL3IOFromL2_inner_charge_[arrSize_];
  double iterL3IOFromL2_inner_trkChi2_[arrSize_];
  double iterL3IOFromL2_inner_validFraction_[arrSize_];
  int    iterL3IOFromL2_inner_trackerLayers_[arrSize_];
  int    iterL3IOFromL2_inner_trackerHits_[arrSize_];
  int    iterL3IOFromL2_inner_lostTrackerHits_[arrSize_];
  int    iterL3IOFromL2_inner_lostTrackerHitsIn_[arrSize_];
  int    iterL3IOFromL2_inner_lostTrackerHitsOut_[arrSize_];
  int    iterL3IOFromL2_inner_lostPixelHits_[arrSize_];
  int    iterL3IOFromL2_inner_lostPixelBarrelHits_[arrSize_];
  int    iterL3IOFromL2_inner_lostPixelEndcapHits_[arrSize_];
  int    iterL3IOFromL2_inner_lostStripHits_[arrSize_];
  int    iterL3IOFromL2_inner_lostStripTIBHits_[arrSize_];
  int    iterL3IOFromL2_inner_lostStripTIDHits_[arrSize_];
  int    iterL3IOFromL2_inner_lostStripTOBHits_[arrSize_];
  int    iterL3IOFromL2_inner_lostStripTECHits_[arrSize_];
  int    iterL3IOFromL2_inner_pixelLayers_[arrSize_];
  int    iterL3IOFromL2_inner_pixelHits_[arrSize_];
  double iterL3IOFromL2_outer_pt_[arrSize_];
  double iterL3IOFromL2_outer_eta_[arrSize_];
  double iterL3IOFromL2_outer_phi_[arrSize_];
  double iterL3IOFromL2_outer_charge_[arrSize_];
  double iterL3IOFromL2_global_pt_[arrSize_];
  double iterL3IOFromL2_global_eta_[arrSize_];
  double iterL3IOFromL2_global_phi_[arrSize_];
  double iterL3IOFromL2_global_charge_[arrSize_];
  int    iterL3IOFromL2_global_muonHits_[arrSize_];
  double iterL3IOFromL2_global_trkChi2_[arrSize_];
  int    iterL3IOFromL2_global_trackerLayers_[arrSize_];
  int    iterL3IOFromL2_global_trackerHits_[arrSize_];

  // -- iterL3 object from outside-in + inside-out step (from L2)
  int    nIterL3FromL2_;
  double iterL3FromL2_inner_pt_[arrSize_];
  double iterL3FromL2_inner_eta_[arrSize_];
  double iterL3FromL2_inner_phi_[arrSize_];
  double iterL3FromL2_inner_charge_[arrSize_];
  double iterL3FromL2_inner_trkChi2_[arrSize_];
  double iterL3FromL2_inner_validFraction_[arrSize_];
  int    iterL3FromL2_inner_trackerLayers_[arrSize_];
  int    iterL3FromL2_inner_trackerHits_[arrSize_];
  int    iterL3FromL2_inner_lostTrackerHits_[arrSize_];
  int    iterL3FromL2_inner_lostTrackerHitsIn_[arrSize_];
  int    iterL3FromL2_inner_lostTrackerHitsOut_[arrSize_];
  int    iterL3FromL2_inner_lostPixelHits_[arrSize_];
  int    iterL3FromL2_inner_lostPixelBarrelHits_[arrSize_];
  int    iterL3FromL2_inner_lostPixelEndcapHits_[arrSize_];
  int    iterL3FromL2_inner_lostStripHits_[arrSize_];
  int    iterL3FromL2_inner_lostStripTIBHits_[arrSize_];
  int    iterL3FromL2_inner_lostStripTIDHits_[arrSize_];
  int    iterL3FromL2_inner_lostStripTOBHits_[arrSize_];
  int    iterL3FromL2_inner_lostStripTECHits_[arrSize_];
  int    iterL3FromL2_inner_pixelLayers_[arrSize_];
  int    iterL3FromL2_inner_pixelHits_[arrSize_];
  double iterL3FromL2_outer_pt_[arrSize_];
  double iterL3FromL2_outer_eta_[arrSize_];
  double iterL3FromL2_outer_phi_[arrSize_];
  double iterL3FromL2_outer_charge_[arrSize_];
  double iterL3FromL2_global_pt_[arrSize_];
  double iterL3FromL2_global_eta_[arrSize_];
  double iterL3FromL2_global_phi_[arrSize_];
  double iterL3FromL2_global_charge_[arrSize_];
  int    iterL3FromL2_global_muonHits_[arrSize_];
  double iterL3FromL2_global_trkChi2_[arrSize_];
  int    iterL3FromL2_global_trackerLayers_[arrSize_];
  int    iterL3FromL2_global_trackerHits_[arrSize_];

  // -- iterL3 object from inside-out step (from L1)
  int    nIterL3IOFromL1_;
  double iterL3IOFromL1_pt_[arrSize_];
  double iterL3IOFromL1_eta_[arrSize_];
  double iterL3IOFromL1_phi_[arrSize_];
  double iterL3IOFromL1_charge_[arrSize_];
  int    iterL3IOFromL1_muonHits_[arrSize_];
  double iterL3IOFromL1_trkChi2_[arrSize_];
  double iterL3IOFromL1_validFraction_[arrSize_];
  int    iterL3IOFromL1_trackerLayers_[arrSize_];
  int    iterL3IOFromL1_trackerHits_[arrSize_];
  int    iterL3IOFromL1_lostTrackerHits_[arrSize_];
  int    iterL3IOFromL1_lostTrackerHitsIn_[arrSize_];
  int    iterL3IOFromL1_lostTrackerHitsOut_[arrSize_];
  int    iterL3IOFromL1_lostPixelHits_[arrSize_];
  int    iterL3IOFromL1_lostPixelBarrelHits_[arrSize_];
  int    iterL3IOFromL1_lostPixelEndcapHits_[arrSize_];
  int    iterL3IOFromL1_lostStripHits_[arrSize_];
  int    iterL3IOFromL1_lostStripTIBHits_[arrSize_];
  int    iterL3IOFromL1_lostStripTIDHits_[arrSize_];
  int    iterL3IOFromL1_lostStripTOBHits_[arrSize_];
  int    iterL3IOFromL1_lostStripTECHits_[arrSize_];
  int    iterL3IOFromL1_pixelLayers_[arrSize_];
  int    iterL3IOFromL1_pixelHits_[arrSize_];

  // -- iterL3 object before applying ID @ HLT
  int nIterL3MuonNoID_;
  double iterL3MuonNoID_pt_[arrSize_];
  double iterL3MuonNoID_innerPt_[arrSize_];
  double iterL3MuonNoID_eta_[arrSize_];
  double iterL3MuonNoID_phi_[arrSize_];
  double iterL3MuonNoID_charge_[arrSize_];
  int    iterL3MuonNoID_isGLB_[arrSize_];
  int    iterL3MuonNoID_isSTA_[arrSize_];
  int    iterL3MuonNoID_isTRK_[arrSize_];
  double iterL3MuonNoID_inner_trkChi2_[arrSize_];
  double iterL3MuonNoID_inner_validFraction_[arrSize_];
  int    iterL3MuonNoID_inner_trackerLayers_[arrSize_];
  int    iterL3MuonNoID_inner_trackerHits_[arrSize_];
  int    iterL3MuonNoID_inner_lostTrackerHits_[arrSize_];
  int    iterL3MuonNoID_inner_lostTrackerHitsIn_[arrSize_];
  int    iterL3MuonNoID_inner_lostTrackerHitsOut_[arrSize_];
  int    iterL3MuonNoID_inner_lostPixelHits_[arrSize_];
  int    iterL3MuonNoID_inner_lostPixelBarrelHits_[arrSize_];
  int    iterL3MuonNoID_inner_lostPixelEndcapHits_[arrSize_];
  int    iterL3MuonNoID_inner_lostStripHits_[arrSize_];
  int    iterL3MuonNoID_inner_lostStripTIBHits_[arrSize_];
  int    iterL3MuonNoID_inner_lostStripTIDHits_[arrSize_];
  int    iterL3MuonNoID_inner_lostStripTOBHits_[arrSize_];
  int    iterL3MuonNoID_inner_lostStripTECHits_[arrSize_];
  int    iterL3MuonNoID_inner_pixelLayers_[arrSize_];
  int    iterL3MuonNoID_inner_pixelHits_[arrSize_];
  int    iterL3MuonNoID_global_muonHits_[arrSize_];
  double iterL3MuonNoID_global_trkChi2_[arrSize_];
  int    iterL3MuonNoID_global_trackerLayers_[arrSize_];
  int    iterL3MuonNoID_global_trackerHits_[arrSize_];
  double iterL3MuonNoID_momentumChi2_[arrSize_];
  double iterL3MuonNoID_positionChi2_[arrSize_];
  double iterL3MuonNoID_glbKink_[arrSize_];
  double iterL3MuonNoID_glbTrackProbability_[arrSize_];
  double iterL3MuonNoID_globalDeltaEtaPhi_[arrSize_];
  double iterL3MuonNoID_localDistance_[arrSize_];
  double iterL3MuonNoID_staRelChi2_[arrSize_];
  int    iterL3MuonNoID_tightMatch_[arrSize_];
  double iterL3MuonNoID_trkKink_[arrSize_];
  double iterL3MuonNoID_trkRelChi2_[arrSize_];
  double iterL3MuonNoID_segmentCompatibility_[arrSize_];

  // -- iterL3 object after applying ID @ HLT
  int nIterL3Muon_;
  double iterL3Muon_pt_[arrSize_];
  double iterL3Muon_innerPt_[arrSize_];
  double iterL3Muon_eta_[arrSize_];
  double iterL3Muon_phi_[arrSize_];
  double iterL3Muon_charge_[arrSize_];
  int    iterL3Muon_isGLB_[arrSize_];
  int    iterL3Muon_isSTA_[arrSize_];
  int    iterL3Muon_isTRK_[arrSize_];
  double iterL3Muon_inner_trkChi2_[arrSize_];
  double iterL3Muon_inner_validFraction_[arrSize_];
  int    iterL3Muon_inner_trackerLayers_[arrSize_];
  int    iterL3Muon_inner_trackerHits_[arrSize_];
  int    iterL3Muon_inner_lostTrackerHits_[arrSize_];
  int    iterL3Muon_inner_lostTrackerHitsIn_[arrSize_];
  int    iterL3Muon_inner_lostTrackerHitsOut_[arrSize_];
  int    iterL3Muon_inner_lostPixelHits_[arrSize_];
  int    iterL3Muon_inner_lostPixelBarrelHits_[arrSize_];
  int    iterL3Muon_inner_lostPixelEndcapHits_[arrSize_];
  int    iterL3Muon_inner_lostStripHits_[arrSize_];
  int    iterL3Muon_inner_lostStripTIBHits_[arrSize_];
  int    iterL3Muon_inner_lostStripTIDHits_[arrSize_];
  int    iterL3Muon_inner_lostStripTOBHits_[arrSize_];
  int    iterL3Muon_inner_lostStripTECHits_[arrSize_];
  int    iterL3Muon_inner_pixelLayers_[arrSize_];
  int    iterL3Muon_inner_pixelHits_[arrSize_];
  int    iterL3Muon_global_muonHits_[arrSize_];
  double iterL3Muon_global_trkChi2_[arrSize_];
  int    iterL3Muon_global_trackerLayers_[arrSize_];
  int    iterL3Muon_global_trackerHits_[arrSize_];
  double iterL3Muon_momentumChi2_[arrSize_];
  double iterL3Muon_positionChi2_[arrSize_];
  double iterL3Muon_glbKink_[arrSize_];
  double iterL3Muon_glbTrackProbability_[arrSize_];
  double iterL3Muon_globalDeltaEtaPhi_[arrSize_];
  double iterL3Muon_localDistance_[arrSize_];
  double iterL3Muon_staRelChi2_[arrSize_];
  int    iterL3Muon_tightMatch_[arrSize_];
  double iterL3Muon_trkKink_[arrSize_];
  double iterL3Muon_trkRelChi2_[arrSize_];
  double iterL3Muon_segmentCompatibility_[arrSize_];

  class tmpTrk {
  private:
    double trkPt;
    double trkEta;
    double trkPhi;
    int trkCharge;
  public:
    bool isMatched(const reco::Track trk_) {
      return std::abs(trk_.pt()-trkPt)/trkPt < 0.01 && std::abs(trk_.eta()-trkEta) < 0.01 && std::abs(trk_.phi()-trkPhi) < 0.01 && trk_.charge()==trkCharge;
    }

    void fill(const reco::TrackRef trk_) {
      trkPt = trk_->pt();
      trkEta = trk_->eta();
      trkPhi = trk_->phi();
      trkCharge = trk_->charge();
    }

    tmpTrk( double dummy = -99999. ) {
      trkPt = dummy;
      trkEta = dummy;
      trkPhi = dummy;
      trkCharge = dummy;
    }

    tmpTrk(const reco::TrackRef trk_) { fill(trk_); }
  };

  class trkTemplate {
  private:
    int nTrks;
    std::vector<double> trkPts;
    std::vector<double> trkEtas;
    std::vector<double> trkPhis;
    std::vector<int> trkCharges;
    std::vector<double> px;
    std::vector<double> py;
    std::vector<double> pz;
    std::vector<double> vx;
    std::vector<double> vy;
    std::vector<double> vz;
    std::vector<double> dxy_bs;
    std::vector<double> dxyError_bs;
    std::vector<double> dz_bs;
    std::vector<double> dzError;
    std::vector<double> trkChi2;
    std::vector<double> validFraction;
    std::vector<int> linkToL3s;
    std::vector<int> linkToL3NoIds;
    std::vector<float> bestMatchTP_charge;
    std::vector<int> bestMatchTP_pdgId;
    std::vector<double> bestMatchTP_energy;
    std::vector<double> bestMatchTP_pt;
    std::vector<double> bestMatchTP_eta;
    std::vector<double> bestMatchTP_phi;
    std::vector<double> bestMatchTP_parentVx;
    std::vector<double> bestMatchTP_parentVy;
    std::vector<double> bestMatchTP_parentVz;
    std::vector<int> bestMatchTP_status;
    std::vector<int> bestMatchTP_numberOfHits;
    std::vector<int> bestMatchTP_numberOfTrackerHits;
    std::vector<int> bestMatchTP_numberOfTrackerLayers;
    std::vector<double> bestMatchTP_sharedFraction;
    std::vector<int> matchedTPsize;
    std::vector<float> mva;

  public:
    void clear() {
      nTrks = 0;
      trkPts.clear();
      trkEtas.clear();
      trkPhis.clear();
      trkCharges.clear();
      px.clear();
      py.clear();
      pz.clear();
      vx.clear();
      vy.clear();
      vz.clear();
      dxy_bs.clear();
      dxyError_bs.clear();
      dz_bs.clear();
      dzError.clear();
      trkChi2.clear();
      validFraction.clear();
      linkToL3s.clear();
      linkToL3NoIds.clear();
      bestMatchTP_charge.clear();
      bestMatchTP_pdgId.clear();
      bestMatchTP_energy.clear();
      bestMatchTP_pt.clear();
      bestMatchTP_eta.clear();
      bestMatchTP_phi.clear();
      bestMatchTP_parentVx.clear();
      bestMatchTP_parentVy.clear();
      bestMatchTP_parentVz.clear();
      bestMatchTP_status.clear();
      bestMatchTP_numberOfHits.clear();
      bestMatchTP_numberOfTrackerHits.clear();
      bestMatchTP_numberOfTrackerLayers.clear();
      bestMatchTP_sharedFraction.clear();
      matchedTPsize.clear();
      mva.clear();

      return;
    }

    void setBranch(TTree* tmpntpl, TString name) {
      tmpntpl->Branch("n"+name, &nTrks);
      tmpntpl->Branch(name+"_pt", &trkPts);
      tmpntpl->Branch(name+"_eta", &trkEtas);
      tmpntpl->Branch(name+"_phi", &trkPhis);
      tmpntpl->Branch(name+"_charge", &trkCharges);
      tmpntpl->Branch(name+"_px", &px);
      tmpntpl->Branch(name+"_py", &py);
      tmpntpl->Branch(name+"_pz", &pz);
      tmpntpl->Branch(name+"_vx", &vx);
      tmpntpl->Branch(name+"_vy", &vy);
      tmpntpl->Branch(name+"_vz", &vz);
      tmpntpl->Branch(name+"_dxy_bs", &dxy_bs);
      tmpntpl->Branch(name+"_dxyError_bs", &dxyError_bs);
      tmpntpl->Branch(name+"_dz_bs", &dz_bs);
      tmpntpl->Branch(name+"_dzError", &dzError);
      tmpntpl->Branch(name+"_trkChi2", &trkChi2);
      tmpntpl->Branch(name+"_validFraction", &validFraction);
      tmpntpl->Branch(name+"_matchedL3", &linkToL3s);
      tmpntpl->Branch(name+"_matchedL3NoId", &linkToL3NoIds);
      tmpntpl->Branch(name+"_bestMatchTP_charge", &bestMatchTP_charge);
      tmpntpl->Branch(name+"_bestMatchTP_pdgId", &bestMatchTP_pdgId);
      tmpntpl->Branch(name+"_bestMatchTP_energy", &bestMatchTP_energy);
      tmpntpl->Branch(name+"_bestMatchTP_pt", &bestMatchTP_pt);
      tmpntpl->Branch(name+"_bestMatchTP_eta", &bestMatchTP_eta);
      tmpntpl->Branch(name+"_bestMatchTP_phi", &bestMatchTP_phi);
      tmpntpl->Branch(name+"_bestMatchTP_parentVx", &bestMatchTP_parentVx);
      tmpntpl->Branch(name+"_bestMatchTP_parentVy", &bestMatchTP_parentVy);
      tmpntpl->Branch(name+"_bestMatchTP_parentVz", &bestMatchTP_parentVz);
      tmpntpl->Branch(name+"_bestMatchTP_status", &bestMatchTP_status);
      tmpntpl->Branch(name+"_bestMatchTP_numberOfHits", &bestMatchTP_numberOfHits);
      tmpntpl->Branch(name+"_bestMatchTP_numberOfTrackerHits", &bestMatchTP_numberOfTrackerHits);
      tmpntpl->Branch(name+"_bestMatchTP_numberOfTrackerLayers", &bestMatchTP_numberOfTrackerLayers);
      tmpntpl->Branch(name+"_bestMatchTP_sharedFraction", &bestMatchTP_sharedFraction);
      tmpntpl->Branch(name+"_matchedTPsize", &matchedTPsize);
      tmpntpl->Branch(name+"_mva", &mva);

      return;
    }

    void fill(const reco::Track& trk, const reco::BeamSpot* bs) {
      trkPts.push_back(trk.pt());
      trkEtas.push_back(trk.eta());
      trkPhis.push_back(trk.phi());
      trkCharges.push_back(trk.charge());
      px.push_back(trk.px());
      py.push_back(trk.py());
      pz.push_back(trk.pz());
      vx.push_back(trk.vx());
      vy.push_back(trk.vy());
      vz.push_back(trk.vz());
      dxy_bs.push_back(trk.dxy(bs->position()));
      dxyError_bs.push_back(trk.dxyError(*bs));
      dz_bs.push_back(trk.dz(bs->position()));
      dzError.push_back(trk.dzError());
      trkChi2.push_back(trk.normalizedChi2());
      validFraction.push_back(trk.validFraction());
      nTrks++;

      return;
    }

    void fillBestTP(const TrackingParticleRef TP) {
      bestMatchTP_charge.push_back(TP->charge());
      bestMatchTP_pdgId.push_back(TP->pdgId());
      bestMatchTP_energy.push_back(TP->energy());
      bestMatchTP_pt.push_back(TP->pt());
      bestMatchTP_eta.push_back(TP->eta());
      bestMatchTP_phi.push_back(TP->phi());
      bestMatchTP_parentVx.push_back(TP->vx());
      bestMatchTP_parentVy.push_back(TP->vy());
      bestMatchTP_parentVz.push_back(TP->vz());
      bestMatchTP_status.push_back(TP->status());
      bestMatchTP_numberOfHits.push_back(TP->numberOfHits());
      bestMatchTP_numberOfTrackerHits.push_back(TP->numberOfTrackerHits());
      bestMatchTP_numberOfTrackerLayers.push_back(TP->numberOfTrackerLayers());

      return;
    }

    void fillDummyTP() {
      bestMatchTP_charge.push_back(-99999.);
      bestMatchTP_pdgId.push_back(-99999);
      bestMatchTP_energy.push_back(-99999.);
      bestMatchTP_pt.push_back(-99999.);
      bestMatchTP_eta.push_back(-99999.);
      bestMatchTP_phi.push_back(-99999.);
      bestMatchTP_parentVx.push_back(-99999.);
      bestMatchTP_parentVy.push_back(-99999.);
      bestMatchTP_parentVz.push_back(-99999.);
      bestMatchTP_status.push_back(-99999);
      bestMatchTP_numberOfHits.push_back(-99999);
      bestMatchTP_numberOfTrackerHits.push_back(-99999);
      bestMatchTP_numberOfTrackerLayers.push_back(-99999);

      return;
    }

    void linkIterL3(int linkNo) { linkToL3s.push_back(linkNo); }
    void linkIterL3NoId(int linkNo) { linkToL3NoIds.push_back(linkNo); }
    int matchedIDpassedL3(int idx) { return linkToL3s.at(idx); }
    void fillBestTPsharedFrac(double frac) { bestMatchTP_sharedFraction.push_back(frac); }
    void fillmatchedTPsize(int TPsize) { matchedTPsize.push_back(TPsize); }
    void fillMva( float mva0_, float mva1_, float mva2_, float mva3_ ) {
      // FIXME tmp solution
      mva.push_back( (mva0_ +0.5) );
    }
  };

  class tpTemplate {
  private:
    int nTP;
    std::vector<float> charge;
    std::vector<int> pdgId;
    std::vector<double> energy;
    std::vector<double> pt;
    std::vector<double> eta;
    std::vector<double> phi;
    std::vector<double> parentVx;
    std::vector<double> parentVy;
    std::vector<double> parentVz;
    std::vector<int> status;
    std::vector<int> numberOfHits;
    std::vector<int> numberOfTrackerHits;
    std::vector<int> numberOfTrackerLayers;
    std::vector<float> gen_charge;
    std::vector<int> gen_pdgId;
    std::vector<double> gen_pt;
    std::vector<double> gen_eta;
    std::vector<double> gen_phi;
    std::vector<double> bestMatchTrk_pt;
    std::vector<double> bestMatchTrk_eta;
    std::vector<double> bestMatchTrk_phi;
    std::vector<int> bestMatchTrk_charge;
    std::vector<double> bestMatchTrk_px;
    std::vector<double> bestMatchTrk_py;
    std::vector<double> bestMatchTrk_pz;
    std::vector<double> bestMatchTrk_vx;
    std::vector<double> bestMatchTrk_vy;
    std::vector<double> bestMatchTrk_vz;
    std::vector<double> bestMatchTrk_dxy_bs;
    std::vector<double> bestMatchTrk_dxyError_bs;
    std::vector<double> bestMatchTrk_dz_bs;
    std::vector<double> bestMatchTrk_dzError;
    std::vector<double> bestMatchTrk_normalizedChi2;
    std::vector<double> bestMatchTrk_quality;
    std::vector<int> bestMatchTrk_NValidHits;
    std::vector<double> bestMatchTrk_mva;
  public:
    void clear() {
      nTP = 0;
      charge.clear();
      pdgId.clear();
      energy.clear();
      pt.clear();
      eta.clear();
      phi.clear();
      parentVx.clear();
      parentVy.clear();
      parentVz.clear();
      status.clear();
      numberOfHits.clear();
      numberOfTrackerHits.clear();
      numberOfTrackerLayers.clear();
      gen_charge.clear();
      gen_pdgId.clear();
      gen_pt.clear();
      gen_eta.clear();
      gen_phi.clear();
      bestMatchTrk_pt.clear();
      bestMatchTrk_eta.clear();
      bestMatchTrk_phi.clear();
      bestMatchTrk_charge.clear();
      bestMatchTrk_px.clear();
      bestMatchTrk_py.clear();
      bestMatchTrk_pz.clear();
      bestMatchTrk_vx.clear();
      bestMatchTrk_vy.clear();
      bestMatchTrk_vz.clear();
      bestMatchTrk_dxy_bs.clear();
      bestMatchTrk_dxyError_bs.clear();
      bestMatchTrk_dz_bs.clear();
      bestMatchTrk_dzError.clear();
      bestMatchTrk_normalizedChi2.clear();
      bestMatchTrk_quality.clear();
      bestMatchTrk_NValidHits.clear();
      bestMatchTrk_mva.clear();

      return;
    }

    void setBranch(TTree* tmpntpl, TString name = "TP") {
      tmpntpl->Branch("n"+name, &nTP);
      tmpntpl->Branch(name+"_charge", &charge);
      tmpntpl->Branch(name+"_pdgId", &pdgId);
      tmpntpl->Branch(name+"_energy", &energy);
      tmpntpl->Branch(name+"_pt", &pt);
      tmpntpl->Branch(name+"_eta", &eta);
      tmpntpl->Branch(name+"_phi", &phi);
      tmpntpl->Branch(name+"_parentVx", &parentVx);
      tmpntpl->Branch(name+"_parentVy", &parentVy);
      tmpntpl->Branch(name+"_parentVz", &parentVz);
      tmpntpl->Branch(name+"_status", &status);
      tmpntpl->Branch(name+"_numberOfHits", &numberOfHits);
      tmpntpl->Branch(name+"_numberOfTrackerHits", &numberOfTrackerHits);
      tmpntpl->Branch(name+"_numberOfTrackerLayers", &numberOfTrackerLayers);
      tmpntpl->Branch(name+"_gen_charge", &gen_charge);
      tmpntpl->Branch(name+"_gen_pdgId", &gen_pdgId);
      tmpntpl->Branch(name+"_gen_pt", &gen_pt);
      tmpntpl->Branch(name+"_gen_eta", &gen_eta);
      tmpntpl->Branch(name+"_gen_phi", &gen_phi);
      tmpntpl->Branch(name+"_bestMatchTrk_pt", &bestMatchTrk_pt);
      tmpntpl->Branch(name+"_bestMatchTrk_eta", &bestMatchTrk_eta);
      tmpntpl->Branch(name+"_bestMatchTrk_phi", &bestMatchTrk_phi);
      tmpntpl->Branch(name+"_bestMatchTrk_charge", &bestMatchTrk_charge);
      tmpntpl->Branch(name+"_bestMatchTrk_px", &bestMatchTrk_px);
      tmpntpl->Branch(name+"_bestMatchTrk_py", &bestMatchTrk_py);
      tmpntpl->Branch(name+"_bestMatchTrk_pz", &bestMatchTrk_pz);
      tmpntpl->Branch(name+"_bestMatchTrk_vx", &bestMatchTrk_vx);
      tmpntpl->Branch(name+"_bestMatchTrk_vy", &bestMatchTrk_vy);
      tmpntpl->Branch(name+"_bestMatchTrk_vz", &bestMatchTrk_vz);
      tmpntpl->Branch(name+"_bestMatchTrk_dxy_bs", &bestMatchTrk_dxy_bs);
      tmpntpl->Branch(name+"_bestMatchTrk_dxyError_bs", &bestMatchTrk_dxyError_bs);
      tmpntpl->Branch(name+"_bestMatchTrk_dz_bs", &bestMatchTrk_dz_bs);
      tmpntpl->Branch(name+"_bestMatchTrk_dzError", &bestMatchTrk_dzError);
      tmpntpl->Branch(name+"_bestMatchTrk_normalizedChi2", &bestMatchTrk_normalizedChi2);
      tmpntpl->Branch(name+"_bestMatchTrk_quality", &bestMatchTrk_quality);
      tmpntpl->Branch(name+"_bestMatchTrk_NValidHits", &bestMatchTrk_NValidHits);
      tmpntpl->Branch(name+"_bestMatchTrk_mva", &bestMatchTrk_mva);

      return;
    }

    void fill(const TrackingParticle TP) {
      charge.push_back(TP.charge());
      pdgId.push_back(TP.pdgId());
      energy.push_back(TP.energy());
      pt.push_back(TP.pt());
      eta.push_back(TP.eta());
      phi.push_back(TP.phi());
      parentVx.push_back(TP.vx());
      parentVy.push_back(TP.vy());
      parentVz.push_back(TP.vz());
      status.push_back(TP.status());
      numberOfHits.push_back(TP.numberOfHits());
      numberOfTrackerHits.push_back(TP.numberOfTrackerHits());
      numberOfTrackerLayers.push_back(TP.numberOfTrackerLayers());

      if( TP.genParticles().empty() ) {
        gen_charge.push_back( -99999. );
        gen_pdgId.push_back( -99999. );
        gen_pt.push_back( -99999. );
        gen_eta.push_back( -99999. );
        gen_phi.push_back( -99999. );
      }
      else {
        gen_charge.push_back( (*TP.genParticles().begin())->charge() );
        gen_pdgId.push_back( (*TP.genParticles().begin())->pdgId() );
        gen_pt.push_back( (*TP.genParticles().begin())->pt() );
        gen_eta.push_back( (*TP.genParticles().begin())->eta() );
        gen_phi.push_back( (*TP.genParticles().begin())->phi() );
      }

      nTP++;

      return;
    }

    void fill_matchedTrk(
      double _pt,
      double _eta,
      double _phi,
      int _charge,
      double _bestMatchTrk_px,
      double _bestMatchTrk_py,
      double _bestMatchTrk_pz,
      double _bestMatchTrk_vx,
      double _bestMatchTrk_vy,
      double _bestMatchTrk_vz,
      double _bestMatchTrk_dxy_bs,
      double _bestMatchTrk_dxyError_bs,
      double _bestMatchTrk_dz_bs,
      double _bestMatchTrk_dzError,
      double _bestMatchTrk_normalizedChi2,
      double _quality,
      int _NValidHits,
      double _mva
    ) {
      bestMatchTrk_pt.push_back(_pt);
      bestMatchTrk_eta.push_back(_eta);
      bestMatchTrk_phi.push_back(_phi);
      bestMatchTrk_charge.push_back(_charge);
      bestMatchTrk_px.push_back(_bestMatchTrk_px);
      bestMatchTrk_py.push_back(_bestMatchTrk_py);
      bestMatchTrk_pz.push_back(_bestMatchTrk_pz);
      bestMatchTrk_vx.push_back(_bestMatchTrk_vx);
      bestMatchTrk_vy.push_back(_bestMatchTrk_vy);
      bestMatchTrk_vz.push_back(_bestMatchTrk_vz);
      bestMatchTrk_dxy_bs.push_back(_bestMatchTrk_dxy_bs);
      bestMatchTrk_dxyError_bs.push_back(_bestMatchTrk_dxyError_bs);
      bestMatchTrk_dz_bs.push_back(_bestMatchTrk_dz_bs);
      bestMatchTrk_dzError.push_back(_bestMatchTrk_dzError);
      bestMatchTrk_normalizedChi2.push_back(_bestMatchTrk_normalizedChi2);
      bestMatchTrk_quality.push_back(_quality);
      bestMatchTrk_NValidHits.push_back(_NValidHits);
      bestMatchTrk_mva.push_back(_mva + 0.5);
    }

  };

  class vtxTemplate {
  private:
    int nVtxs;
    std::vector<int> isValid;
    std::vector<double> chi2;
    std::vector<double> ndof;
    std::vector<double> nTracks;
    std::vector<double> vtxX;
    std::vector<double> vtxXerr;
    std::vector<double> vtxY;
    std::vector<double> vtxYerr;
    std::vector<double> vtxZ;
    std::vector<double> vtxZerr;
  public:
    void clear() {
      nVtxs = 0;
      isValid.clear();
      chi2.clear();
      ndof.clear();
      nTracks.clear();
      vtxX.clear();
      vtxXerr.clear();
      vtxY.clear();
      vtxYerr.clear();
      vtxZ.clear();
      vtxZerr.clear();

      return;
    }

    void setBranch(TTree* tmpntpl, TString name) {
      tmpntpl->Branch("n"+name, &nVtxs);
      tmpntpl->Branch(name+"_isValid", &isValid);
      tmpntpl->Branch(name+"_chi2", &chi2);
      tmpntpl->Branch(name+"_ndof", &ndof);
      tmpntpl->Branch(name+"_nTracks", &nTracks);
      tmpntpl->Branch(name+"_x", &vtxX);
      tmpntpl->Branch(name+"_xerr", &vtxXerr);
      tmpntpl->Branch(name+"_y", &vtxY);
      tmpntpl->Branch(name+"_yerr", &vtxYerr);
      tmpntpl->Branch(name+"_z", &vtxZ);
      tmpntpl->Branch(name+"_zerr", &vtxZerr);

      return;
    }

    void fill(const reco::Vertex vtx) {
      isValid.push_back(vtx.isValid());
      chi2.push_back(vtx.chi2());
      ndof.push_back(vtx.ndof());
      nTracks.push_back(vtx.nTracks());
      vtxX.push_back(vtx.x());
      vtxXerr.push_back(vtx.xError());
      vtxY.push_back(vtx.y());
      vtxYerr.push_back(vtx.yError());
      vtxZ.push_back(vtx.z());
      vtxZerr.push_back(vtx.zError());
      nVtxs++;

      return;
    }
  };

  class hitTemplate {
  private:
    int nHits;
    std::vector<bool> isValid;
    std::vector<float> localx, localy, localz, globalx, globaly, globalz;
  public:
    void clear() {
      nHits = 0;
      isValid.clear();
      localx.clear(); localy.clear(); localz.clear(); globalx.clear(); globaly.clear(); globalz.clear();

      return;
    }

    void fill(int idx, TrackingRecHit* rechit) {
      isValid.push_back(rechit->isValid());
      localx.push_back(rechit->localPosition().x());
      localy.push_back(rechit->localPosition().y());
      localz.push_back(rechit->localPosition().z());
      globalx.push_back(rechit->globalPosition().x());
      globaly.push_back(rechit->globalPosition().y());
      globalz.push_back(rechit->globalPosition().z());

      return;
    }

    void setBranch(TTree* tmpntpl, TString name) {
      tmpntpl->Branch(name+"_isValid", &isValid);
      tmpntpl->Branch(name+"_localX", &localx);
      tmpntpl->Branch(name+"_localY", &localy);
      tmpntpl->Branch(name+"_localZ", &localz);
      tmpntpl->Branch(name+"_globalX", &globalx);
      tmpntpl->Branch(name+"_globalY", &globaly);
      tmpntpl->Branch(name+"_globalZ", &globalz);

      return;
    }
  };

  std::vector<tmpTrk> iterL3IDpassed;
  std::vector<tmpTrk> iterL3NoIDpassed;

  tpTemplate* TrkParticle = new tpTemplate();

  vtxTemplate* VThltIterL3MuonTrimmedPixelVertices = new vtxTemplate();
  vtxTemplate* VThltIterL3FromL1MuonTrimmedPixelVertices = new vtxTemplate();

  vector< trkTemplate* > trkTemplates_;
  vector< tpTemplate* >  tpTemplates_;

  void fill_trackTemplate(
    const edm::Event &iEvent,
    edm::EDGetTokenT<edm::View<reco::Track>>& trkToken,
    edm::EDGetTokenT<reco::RecoToSimCollection>& assoToken,
    trkTemplate* TTtrack,
    bool
  );

  void fill_trackTemplate(
    const edm::Event &iEvent,
    edm::EDGetTokenT<edm::View<reco::Track>>& trkToken,
    edm::EDGetTokenT<reco::RecoToSimCollection>& assoToken,
    const TrackerGeometry& tracker,
    const pairSeedMvaEstimator& pairMvaEstimator,
    trkTemplate* TTtrack,
    bool
  );

  void fill_tpTemplate(
    const edm::Event &iEvent,
    edm::EDGetTokenT<reco::SimToRecoCollection>& assoToken,
    tpTemplate* TTtp
  );

  void fill_tpTemplate(
    const edm::Event &iEvent,
    edm::EDGetTokenT<reco::SimToRecoCollection>& assoToken,
    const TrackerGeometry& tracker,
    const pairSeedMvaEstimator& pairMvaEstimator,
    tpTemplate* TTtp
  );

  void Fill_TP( const edm::Event &iEvent, tpTemplate* TrkParticle );

  // -- seed MVA -- //
  edm::FileInPath mvaFileHltIter2IterL3MuonPixelSeeds_B_;
  edm::FileInPath mvaFileHltIter2IterL3FromL1MuonPixelSeeds_B_;
  edm::FileInPath mvaFileHltIter2IterL3MuonPixelSeeds_E_;
  edm::FileInPath mvaFileHltIter2IterL3FromL1MuonPixelSeeds_E_;

  std::vector<double> mvaScaleMeanHltIter2IterL3MuonPixelSeeds_B_;
  std::vector<double> mvaScaleMeanHltIter2IterL3FromL1MuonPixelSeeds_B_;
  std::vector<double> mvaScaleStdHltIter2IterL3MuonPixelSeeds_B_;
  std::vector<double> mvaScaleStdHltIter2IterL3FromL1MuonPixelSeeds_B_;
  std::vector<double> mvaScaleMeanHltIter2IterL3MuonPixelSeeds_E_;
  std::vector<double> mvaScaleMeanHltIter2IterL3FromL1MuonPixelSeeds_E_;
  std::vector<double> mvaScaleStdHltIter2IterL3MuonPixelSeeds_E_;
  std::vector<double> mvaScaleStdHltIter2IterL3FromL1MuonPixelSeeds_E_;

  pairSeedMvaEstimator mvaHltIter2IterL3MuonPixelSeeds_;
  pairSeedMvaEstimator mvaHltIter2IterL3FromL1MuonPixelSeeds_;

  vector<double> getSeedMva(
    const pairSeedMvaEstimator& pairMvaEstimator,
    const TrajectorySeed& seed,
    GlobalVector global_p,
    const l1t::MuonBxCollection& l1Muons,
    const reco::RecoChargedCandidateCollection& l2Muons
  ) {
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
    if( v_mva.size() != 1 ) {
      cout << "getSeedMva: v_mva.size() != 1" << endl;
    //  return { -99999., -99999., -99999., -99999. };
    }

    return v_mva;
  }
};
