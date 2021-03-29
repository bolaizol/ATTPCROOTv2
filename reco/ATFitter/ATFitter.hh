#ifndef ATFITTER_H
#define ATFITTER_H

#include "ATDigiPar.hh"
#include "ATTrack.hh"
#include "ATEvent.hh"
#include "ATPatternEvent.hh"

// FairRoot classes
#include "FairRootManager.h"
#include "FairLogger.h"

namespace ATFITTER{

      class ATFitter : public TObject
      {

          public:

	    ATFitter();
            virtual ~ATFitter();
            //virtual std::vector<ATTrack> GetFittedTrack() = 0;
            virtual bool FitTracks(ATPatternEvent &patternEvent) = 0;
            virtual void Init()=0;

	  protected:
            FairLogger *fLogger;       								   ///< logger pointer
            ATDigiPar  *fPar;                                                                      ///< parameter container
            std::tuple<Double_t,Double_t>   GetMomFromBrho(Double_t A, Double_t Z, Double_t brho); ///< Returns momentum (in GeV) from Brho assuming M (amu) and Z;
            

            ClassDef(ATFitter, 1)
      };

}//namespace


#endif


