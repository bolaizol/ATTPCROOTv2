#include "ATGenfit.hh"

// FairRoot classes
#include "FairRuntimeDb.h"
#include "FairRun.h"
#include "FairRunAna.h"

#include "TGeoManager.h"

#define cRED "\033[1;31m"
#define cYELLOW "\033[1;33m"
#define cNORMAL "\033[0m"
#define cGREEN "\033[1;32m"

ClassImp(ATFITTER::ATGenfit)

ATFITTER::ATGenfit::ATGenfit()
{

  fTPCDetID = 0;
  fCurrentDirection = 1;

  fKalmanFitter = new genfit::KalmanFitterRefTrack();
  fKalmanFitter -> setMinIterations(5);
  fKalmanFitter -> setMaxIterations(20);

  fGenfitTrackArray = new TClonesArray("genfit::Track");
  fHitClusterArray = new TClonesArray("ATHitCluster");

  fMeasurementProducer = new genfit::MeasurementProducer<ATHitCluster, genfit::ATSpacepointMeasurement>(fHitClusterArray);
  fMeasurementFactory = new genfit::MeasurementFactory<genfit::AbsMeasurement>();
  fMeasurementFactory -> addProducer(fTPCDetID, fMeasurementProducer);

  genfit::FieldManager::getInstance() -> init(new genfit::ConstField(0., 0., 20.)); //TODO kGauss
  genfit::MaterialEffects *materialEffects = genfit::MaterialEffects::getInstance();
  materialEffects -> init(new genfit::TGeoMaterialInterface());

  fPDGCandidateArray = new std::vector<Int_t>;//TODO
  fPDGCandidateArray -> push_back(2212);

  std::cout<<" ATFITTER::ATGenfit::ATGenfit(): Checking materials that GENFIT will use "<<"\n";
  
  TGeoManager* gGeoMan  = (TGeoManager*)gROOT->FindObject("FAIRGeom");
  TObjArray* volume_list = gGeoMan->GetListOfVolumes();
  if (!volume_list) {        
         std::cout<<cRED<< " Warning! Null list of geometry volumes."<<cNORMAL<<"\n";        
   }

  int numVol = volume_list->GetEntries();

   for (int ivol = 0; ivol < numVol; ivol++) {
        TGeoVolume * volume = dynamic_cast <TGeoVolume *>(volume_list->At(ivol));
         if (!volume) {
            
              std::cout<< "Got a null geometry volume!! Skiping current list element"<<"\n";
            continue;
         }
        
         std::cout<<cGREEN<<" Volume name : "<<volume->GetName()<<cNORMAL<<"\n";         

         TGeoMaterial * mat = volume->GetMedium()->GetMaterial();
        
         Int_t mat_indx = mat->GetIndex();
        
     	  if (mat->IsMixture()) {
            TGeoMixture * mixt = dynamic_cast <TGeoMixture*> (mat);
             int Nelements = mixt->GetNelements();
             std::cout<<cYELLOW<<" - Material : "<<mat->GetName()<<cNORMAL<<"\n";
          }else{
             std::cout<<cYELLOW<<" - Material : "<<mat->GetName()<<cNORMAL<<"\n";
	  } 
     }
   

}

ATFITTER::ATGenfit::~ATGenfit()
{

}

void ATFITTER::ATGenfit::Init()
{
  std::cout<<" ATFITTER::ATGenfit::Init() "<<"\n"; 
  fHitClusterArray -> Delete();
  fGenfitTrackArray -> Delete();
}

void ATFITTER::ATGenfit::SetMinIterations(Int_t value) { 
  fKalmanFitter -> setMinIterations(value);
}
void ATFITTER::ATGenfit::SetMaxIterations(Int_t value) { 
  fKalmanFitter -> setMaxIterations(value);
}

bool ATFITTER::ATGenfit::FitTracks(ATPatternEvent &patternEvent) //TODO Change return value
{

  fHitClusterArray -> Delete();
  genfit::TrackCand trackCand;
  
  std::vector<ATTrack>& patternTrackCand =  patternEvent.GetTrackCand();


  std::cout<<" ATFITTER::ATGenfit::FitTracks - Number of candidate tracks : "<<patternTrackCand.size()<<"\n";

  for(auto track : patternTrackCand)
  {
	auto hitClusterArray = track.GetHitClusterArray();


        if(hitClusterArray->size()>3){// TODO Check minimum number of clusters

                 //Adding clusterized  hits
		 for (auto cluster : *hitClusterArray) { 
		    Int_t idx = fHitClusterArray->GetEntriesFast();
		    new ((*fHitClusterArray)[idx]) ATHitCluster(cluster);
		    trackCand.addHit(fTPCDetID, idx);
	  	 }  

		 TVector3 posSeed = hitClusterArray->at(0).GetPosition(); //TODO Check first cluster is the first in time 
		 posSeed.SetMag(posSeed.Mag()/10.);

                 TMatrixDSym covSeed(6);//TODO Check where COV matrix is defined, likely in ATPattern clusterize (hard coded in ATSpacePoint measurement)
		 TMatrixD covMatrix = hitClusterArray->at(0).GetCovMatrix();
		  for (Int_t iComp = 0; iComp < 3; iComp++)
		    covSeed(iComp, iComp) = covMatrix(iComp, iComp)/100.;// unit conversion mm2 -> cm2
		   
		  for (Int_t iComp = 3; iComp < 6; iComp++)
		    covSeed(iComp, iComp) = covSeed(iComp - 3, iComp - 3);
  
        
                  //Initial parameters from pattern recognition. For the moment, I just hardcode the paramters for a specific case. Momentum will be determined from BRho.
                  //Test case 12Be+p elastic 15A MeV, 80 deg, 1.5 MeV, 2 T, 0.17704 Tm
		  Double_t theta  = 80.0*TMath::DegToRad();//track->GetGeoTheta();
                  Double_t radius = 0.0;//track->GetGeoRadius();
		  Double_t phi    = 0.0;//track->GetGeoPhi();
                 
		  Double_t brho = 0.17704; //1.5 MeV proton
                  Double_t p_mass = 1.00727647; //amu
                  Int_t p_Z = 1;

                  std::tuple<Double_t,Double_t> mom_ener = GetMomFromBrho(p_mass,p_Z,brho); //TODO Change to structured bindings when C++17		   
		  Double_t momSeedMag = std::get<0>(mom_ener);
		  TVector3 momSeed(0., 0., momSeedMag); // 
		  momSeed.SetTheta(theta);//TODO: Check angle conventions
                  momSeed.SetPhi(phi); // TODO
                  trackCand.setCovSeed(covSeed);
  		  trackCand.setPosMomSeed(posSeed, momSeed,p_Z);

                  genfit::Track *gfTrack = new ((*fGenfitTrackArray)[fGenfitTrackArray -> GetEntriesFast()]) genfit::Track(trackCand, *fMeasurementFactory);
                  gfTrack -> addTrackRep(new genfit::RKTrackRep(2212));// TODO: Forcing proton track representation

                  genfit::RKTrackRep *trackRep = (genfit::RKTrackRep *) gfTrack -> getTrackRep(0);

                  try {
    			fKalmanFitter -> processTrackWithRep(gfTrack, trackRep, false);
  		  } catch (genfit::Exception &e) {}


                  genfit::FitStatus *fitStatus;
		  try {
		    fitStatus = gfTrack -> getFitStatus(trackRep);
                    std::cout<<" Is fitted? "<<fitStatus->isFitted()<<"\n";
		  } catch (genfit::Exception &e) {
		    return 0;
		  }

		  genfit::MeasuredStateOnPlane fitState;
	          try {
		    fitState = gfTrack -> getFittedState();
	          } catch (genfit::Exception &e) {}

		  




        }//ClusterArray

  }//iTrack
 


}
