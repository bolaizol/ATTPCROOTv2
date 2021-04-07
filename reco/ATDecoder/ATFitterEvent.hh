#ifndef ATFITTEREVENT_H
#define ATFITTEREVENT_H

#include "TROOT.h"
#include "TObject.h"
#include "TVector3.h"

#include <vector>
#include <map>

#include "ATHit.hh"
#include "ATHitCluster.hh"
#include "ATSpacePointMeasurement.hh"
#include "ATTrack.hh"

// GENFIT2 classes
#include "AbsKalmanFitter.h"
#include "KalmanFitterRefTrack.h"
#include "DAF.h"
#include "ConstField.h"
#include "FieldManager.h"
#include "MaterialEffects.h"
#include "TGeoMaterialInterface.h"
#include "MeasurementFactory.h"
#include "MeasurementProducer.h"
#include "EventDisplay.h"

class ATFitterEvent : public TNamed {

  public:
    ATFitterEvent();
    ~ATFitterEvent();

    void SetTrackArray(std::vector<ATTrack> *trackArray);
    void SetTrack(ATTrack *track);
    
    std::vector<ATTrack> GetTrackArray();


  private:

   std::vector<ATTrack> fTrackArray;
   

   ClassDef(ATFitterEvent, 1);

 };

 #endif
