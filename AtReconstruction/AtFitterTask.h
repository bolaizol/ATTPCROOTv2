/*********************************************************************
 *   Fitter Task AtFitterTask.hh			             *
 *   Author: Y. Ayyad ayyadlim@frib.msu.edu            	             *
 *   Log: 3/10/2021 					             *
 *								     *
 *********************************************************************/

#ifndef ATFITTERTASK
#define ATFITTERTASK

// FAIRROOT classes
#include "FairTask.h"
#include "FairLogger.h"

#include "AtFitter.h"
#include "AtGenfit.h"

// GENFIT2 classes
#include "AbsKalmanFitter.h"
#include "KalmanFitterRefTrack.h"
#include "DAF.h"
//#include "GFRaveVertexFactory.h"
#include "ConstField.h"
#include "FieldManager.h"
#include "MaterialEffects.h"
#include "TGeoMaterialInterface.h"
#include "MeasurementFactory.h"
#include "MeasurementProducer.h"
#include "EventDisplay.h"

#include "TClonesArray.h"

class AtFitterTask : public FairTask {

public:
   AtFitterTask();
   ~AtFitterTask();

   virtual InitStatus Init();
   virtual void SetParContainers();
   virtual void Exec(Option_t *opt);

   void SetPersistence(Bool_t value = kTRUE);
   void SetFitterAlgorithm(Int_t value = 0);

private:
   Bool_t fIsPersistence; //!< Persistence check variable
   FairLogger *fLogger;
   AtDigiPar *fPar;
   TClonesArray *fEventHArray;
   AtFITTER::AtFitter *fFitter;
   Int_t fFitterAlgorithm;

   ClassDef(AtFitterTask, 1);
};

#endif
