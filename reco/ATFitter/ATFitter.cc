#include "ATFitter.hh"

// FairRoot classes
#include "FairRuntimeDb.h"
#include "FairRun.h"

// STL
#include <algorithm>



ClassImp(ATFITTER::ATFitter)


ATFITTER::ATFitter::ATFitter()
{
  

}

ATFITTER::ATFitter::~ATFitter()
{


}

std::tuple<Double_t,Double_t>   ATFITTER::ATFitter::GetMomFromBrho(Double_t M, Double_t Z, Double_t brho)
{
 
  const Double_t M_Ener = M*931.49401;
  Double_t p = brho*Z*(2.99792458/10.0); //In GeV
  Double_t E = TMath::Sqrt(TMath::Power(p,2) + TMath::Power(M_Ener,2)) - M_Ener;
  return std::make_tuple(p,E); 
  
 
}
