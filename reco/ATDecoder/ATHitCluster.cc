#include "ATHitCluster.hh"

#include "TMath.h"

#include <iostream>

ClassImp(ATHitCluster);

ATHitCluster::ATHitCluster()
{
  Clear();
}

ATHitCluster::ATHitCluster(ATHitCluster *cluster)
{
  fIsClustered = cluster -> IsClustered();
  fClusterID = cluster -> GetClusterID();
  fTrackID = cluster -> GetTrackID();
  
  fCovMatrix.ResizeTo(3, 3);
  fCovMatrix = cluster -> GetCovMatrix();
  fCharge  = cluster -> GetCharge();
  fLength = cluster->GetLength();

  SetPOCA(cluster -> GetPOCA());
  SetPosition(cluster -> GetPosition());
  SetTimeStamp(cluster -> GetTimeStamp());
  SetPosSigma(cluster->GetPosSigma());

}

void ATHitCluster::Clear(Option_t *)
{

  fClusterID = -1;

  fCharge = 0;

  fCovMatrix.ResizeTo(3, 3);
  for (Int_t iElem = 0; iElem < 9; iElem++)
    fCovMatrix(iElem/3, iElem%3) = 0;


  fLength = -999;

  fPOCAX = -999;
  fPOCAY = -999;
  fPOCAZ = -999;

  //fX = 0;
  //fY = 0;
  //fZ = 0;

 
}

void     ATHitCluster::SetCovMatrix(TMatrixD matrix) { fCovMatrix = matrix; }
void     ATHitCluster::SetLength(Double_t length)    { fLength = length;} 
void     ATHitCluster::SetPOCA(TVector3 p)
{
  fPOCAX = p.X();
  fPOCAY = p.Y();
  fPOCAZ = p.Z();
}

TVector3 ATHitCluster::GetPOCA()                     { return TVector3(fPOCAX, fPOCAY, fPOCAZ); }
Double_t ATHitCluster::GetLength()                   { return fLength; }
TMatrixD ATHitCluster::GetCovMatrix()         const  { return fCovMatrix; }

