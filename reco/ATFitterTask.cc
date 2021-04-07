#include "ATFitterTask.hh"

// ATTPCROOT classes
#include "ATEvent.hh"
#include "ATTrack.hh"

// FAIRROOT classes
#include "FairRootManager.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"

// GENFIT2 classes
#include "Track.h"
#include "TrackCand.h"
#include "RKTrackRep.h"
#include "Exception.h"


// STL
#include <iostream>

// ROOT classes
#include "TMatrixDSym.h"
#include "TMatrixD.h"
#include "TMath.h"
#include "TGeoManager.h"
#include "Math/DistFunc.h"

ClassImp(ATFitterTask);

ATFitterTask::ATFitterTask()
{
  fLogger = FairLogger::GetLogger();
  fPar = NULL;
  fIsPersistence = kFALSE;
  fPatternEventArray = new TClonesArray("ATPatternEvent");
  fGenfitTrackArray  = new TClonesArray("genfit::Track");
  fFitterAlgorithm = 0;

  fGenfitTrackVector = new std::vector<genfit::Track>();

}

ATFitterTask::~ATFitterTask()
{

}

void ATFitterTask::SetPersistence(Bool_t value) { fIsPersistence = value; }


InitStatus ATFitterTask::Init()
{
  FairRootManager *ioMan = FairRootManager::Instance();
  if (ioMan == 0) {
    fLogger -> Error(MESSAGE_ORIGIN, "Cannot find RootManager!");
    return kERROR;
  }

  fPatternEventArray = (TClonesArray *) ioMan -> GetObject("ATPatternEvent");
    if (fPatternEventArray == 0) {
      fLogger -> Error(MESSAGE_ORIGIN, "Cannot find ATPatternEvent array!");
      return kERROR;
  }

  //Algorithm selection
  
    if (fFitterAlgorithm == 0) {
      fLogger -> Info(MESSAGE_ORIGIN, "Using GENFIT2!");

      fFitter = new ATFITTER::ATGenfit();

    } else if (fFitterAlgorithm == 1) {
      fLogger -> Error(MESSAGE_ORIGIN, "Fitter algorithm not defined!");
      return kERROR;
    } else if (fFitterAlgorithm == 2) {
      fLogger -> Error(MESSAGE_ORIGIN, "Fitter algorithm not defined!");
      return kERROR;
    }

   //ioMan -> Register("genfitTrackTCA","ATTPC",fGenfitTrackArray, fIsPersistence);
   ioMan -> RegisterAny("ATTPC",fGenfitTrackVector, fIsPersistence);


  return kSUCCESS;
}

void ATFitterTask::SetParContainers()
{
    fLogger->Debug(MESSAGE_ORIGIN, "SetParContainers of ATFitterTask");

    FairRun *run = FairRun::Instance();
    if (!run)
      fLogger -> Fatal(MESSAGE_ORIGIN, "No analysis run!");

    FairRuntimeDb *db = run -> GetRuntimeDb();
    if (!db)
      fLogger -> Fatal(MESSAGE_ORIGIN, "No runtime database!");

    fPar = (ATDigiPar *) db -> getContainer("ATDigiPar");
    if (!fPar)
      fLogger -> Fatal(MESSAGE_ORIGIN, "ATDigiPar not found!!");
}

void ATFitterTask::Exec(Option_t* option)
{

   if (fPatternEventArray -> GetEntriesFast() == 0)
    return;

   fGenfitTrackArray->Delete();
   fGenfitTrackVector->clear();

   fFitter->Init();

   ATPatternEvent &patternEvent = *((ATPatternEvent*) fPatternEventArray->At(0));

   fFitter->FitTracks(patternEvent);

   //TODO: Genfit block, add a dynamic cast and a try-catch

    try{
	   auto genfitTrackArray = dynamic_cast<ATFITTER::ATGenfit*>(fFitter)->GetGenfitTrackArray();
	   auto genfitTracks = genfitTrackArray->GetEntriesFast();
    
  	 for(auto iTrack=0;iTrack<genfitTracks;++iTrack){
		 new ((*fGenfitTrackArray)[iTrack]) genfit::Track(*static_cast<genfit::Track*>(genfitTrackArray->At(iTrack)));
                 //auto trackTest = *static_cast<genfit::Track*>(genfitTrackArray->At(iTrack));
                 //trackTest.Print();
                 //genfit::MeasuredStateOnPlane fitState = trackTest.getFittedState();
                 //fitState.Print();
                 fGenfitTrackVector->push_back(*static_cast<genfit::Track*>(genfitTrackArray->At(iTrack)));
         }

         /*auto genfitTracks_ = fGenfitTrackArray->GetEntriesFast();   

         for(auto iTrack=0;iTrack<genfitTracks_;++iTrack){
		 
                 auto trackTest = *static_cast<genfit::Track*>(fGenfitTrackArray->At(iTrack));
                 trackTest.Print();
                 genfit::MeasuredStateOnPlane fitState = trackTest.getFittedState();
                 fitState.Print();
         } */      

        

    }catch(std::exception& e){
         std::cout<<" "<< e.what()<<"\n";
    }

     

}























