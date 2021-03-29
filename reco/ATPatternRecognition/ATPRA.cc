#include "ATPRA.hh"

// FairRoot classes
#include "FairRuntimeDb.h"
#include "FairRun.h"

// STL
#include <algorithm>

ClassImp(ATPATTERN::ATPRA)


ATPATTERN::ATPRA::~ATPRA()
{
}

void ATPATTERN::ATPRA::SetTrackCurvature(ATTrack& track)
{
        std::cout<<" new track  "<<"\n";
        std::vector<double> radius_vec;
        std::vector<ATHit>* hitArray = track.GetHitArray();
        int nstep = 0.60*hitArray->size(); //20% of the hits to calculate the radius of curvature with less fluctuations
        

        for(Int_t iHit=0; iHit<(hitArray->size()-nstep); iHit++){

          ATHit hitA = hitArray->at(iHit);
          ATHit hitB = hitArray->at((int)(iHit+(nstep/2.0)));
          ATHit hitC = hitArray->at((int)(iHit+nstep));

          //std::cout<<nstep<<" "<<iHit<<"  "<<(int)(iHit+(nstep/2.0))<<"  "<<(int)(iHit+nstep)<<"\n";

          TVector3 posA = hitA.GetPosition();
          TVector3 posB = hitB.GetPosition();
          TVector3 posC = hitC.GetPosition();

          double slopeAB = ( posB.Y()-posA.Y() )/( posB.X()-posA.X() ); //m1
          double slopeBC = ( posC.Y()-posB.Y() )/( posC.X()-posB.X() ); //m2

          double centerX = ( slopeAB*slopeBC*( posA.Y()-posC.Y() ) + slopeBC*( posB.X()+posA.X() )
             - slopeAB*(posB.X()+posC.X()) )/ ( 2.0*(slopeBC - slopeAB) );

          double centerY = (-1/slopeAB)*( centerX - ( posB.X()+posA.X() )/2.0 )  + ( posB.Y()+posA.Y() )/2.0;

          //std::cout<<" Center "<<centerX<<" - "<<centerY<<"\n";

          double radiusA = TMath::Sqrt( TMath::Power(posA.X() - centerX,2) + TMath::Power(posA.Y() - centerY,2)   );
          radius_vec.push_back(radiusA);
          double radiusB = TMath::Sqrt( TMath::Power(posB.X() - centerX,2) + TMath::Power(posB.Y() - centerY,2)   );
          radius_vec.push_back(radiusB);
          double radiusC = TMath::Sqrt( TMath::Power(posC.X() - centerX,2) + TMath::Power(posC.Y() - centerY,2)   );
          radius_vec.push_back(radiusC);




        }


}

void ATPATTERN::ATPRA::SetTrackInitialParameters(ATTrack& track)
{

      //Get the radius of curvature from RANSAC
      ATRANSACN::ATRansac RansacSmoothRadius;
      RansacSmoothRadius.SetModelType(pcl::SACMODEL_CIRCLE2D);
      RansacSmoothRadius.SetRANSACPointThreshold(0.1);
      RansacSmoothRadius.SetDistanceThreshold(6.0);
      std::vector<ATTrack*> circularTracks = RansacSmoothRadius.Ransac(track.GetHitArray()); //Only part of the spiral is used
                                                                               //This function also sets the coefficients
                                                                               //i.e. radius of curvature and center

     // if(circularTracks[0]->GetHitArray() != nullptr) 

  

      if(!circularTracks.empty())
      {  


          std::vector<ATHit>* hits = circularTracks[0]->GetHitArray();

          std::vector<Double_t> coeff = circularTracks[0]->GetRANSACCoeff();

          std::vector<double> wpca;
          std::vector<double> whit;
          std::vector<double> arclength;

          TGraph *arclengthGraph = new TGraph();

          TVector3 posPCA = hits->at(0).GetPosition();

          std::vector<ATHit>* thetaHits = new std::vector<ATHit>();

          for (size_t i = 0; i < hits->size (); ++i){

                  TVector3 pos = hits->at(i).GetPosition();


                  //std::cerr << inliers->indices[i] << "    " << cloud->points[inliers->indices[i]].x << " "
                                                             //<< cloud->points[inliers->indices[i]].y << " "
                                                             //<< cloud->points[inliers->indices[i]].z << std::endl;

                   wpca.push_back( TMath::ATan2 ( posPCA.Y() -  coeff.at(1) , posPCA.X() -  coeff.at(0) ) );    

                   whit.push_back( TMath::ATan2 ( pos.Y() -  coeff.at(1) , pos.X() -  coeff.at(0) ) );

                   arclength.push_back(  fabs(coeff.at(2))*( wpca.at(i) - whit.at(i))  );                                   

                   arclengthGraph->SetPoint(arclengthGraph->GetN(),arclength.at(i),pos.Z());

                   //std::cout<<pos.X()<<"  "<<pos.Y()<<" "<<pos.Z()<<" "<<hits->at(i).GetTimeStamp()<<"  "<<hits->at(i).GetCharge()<<"\n";


                   //Add a hit in the Arc legnth - Z plane
                   double xPos = arclength.at(i);
                   double yPos = pos.Z();
                   double zPos = 0.0;

                   thetaHits->push_back(ATHit(hits->at(i).GetHitPadNum(),i, xPos, yPos,zPos,hits->at(i).GetCharge()) );


           }

           TF1 *f1 = new TF1("f1","pol1",-500,500);
        // TF1 * f1 = new TF1("f1",[](double *x, double *p) { return (p[0]+p[1]*x[0]); },-500,500,2); 
        // TF1 * f1 = new TF1("f1","[0]+[1]*x",-500,500);
        // TF1 * f1 = new TF1("f1",fitf,-500,500,2);
        // arclengthGraph->Fit(f1,"R");  
        // Double_t slope = f1->GetParameter(1);
        // std::cout<<" Slope "<<slope<<"\n";

         double slope = 0;

         //std::cout<<" RANSAC Theta "<<"\n";
         ATRANSACN::ATRansac RansacTheta;
         RansacTheta.SetModelType(pcl::SACMODEL_LINE);
         RansacTheta.SetRANSACPointThreshold(0.1);
         RansacTheta.SetDistanceThreshold(6.0);
         std::vector<ATTrack*> thetaTracks = RansacTheta.Ransac(thetaHits);

         //RansacTheta.MinimizeTrack(thetaTracks[0]);

         double angle = (TMath::ATan2(slope,1)*180.0/TMath::Pi());


         if(angle<0) angle=90.0+angle;
         else if(angle>0) angle=90+angle;

         track.SetGeoCenter(std::make_pair(coeff.at(0),coeff.at(1)));
         track.SetGeoTheta(angle*TMath::Pi()/180.0);

         //delete f1;
         delete arclengthGraph;
         delete thetaHits;

      } 
     

}

void ATPATTERN::ATPRA::Clusterize(ATTrack& track)
{
        //std::cout<<" ================================================================= "<<"\n";
        //std::cout<<" Clusterizing track : "<<track.GetTrackID()<<"\n"; 
        std::vector<ATHit>* hitArray = track.GetHitArray();
        std::vector<ATHit> hitTBArray;
        int clusterID = 0;

        /*for(auto iHits=0;iHits<hitArray->size();++iHits)
	  {
	    TVector3 pos    = hitArray->at(iHits).GetPosition();
            double Q = hitArray->at(iHits).GetCharge();
            int TB          = hitArray->at(iHits).GetTimeStamp();
	    std::cout<<" Pos : "<<pos.X()<<" - "<<pos.Y()<<" - "<<pos.Z()<<" - TB : "<<TB<<" - Charge : "<<Q<<"\n";	

	  }*/       
               

	  for(auto iTB=0;iTB<512;++iTB){

		Double_t clusterQ = 0.0;
                hitTBArray.clear();

        	std::copy_if(hitArray->begin(), hitArray->end(), std::back_inserter(hitTBArray),[&iTB](ATHit& hit){return hit.GetTimeStamp()==iTB;} );

		if(hitTBArray.size()>0)
		{
                  double x=0,y =0;
		  std::shared_ptr<ATHitCluster> hitCluster = std::make_shared<ATHitCluster>();
                  hitCluster->SetIsClustered();
                  hitCluster->SetClusterID(clusterID);
                  Double_t hitQ = 0.0;
                  std::for_each(hitTBArray.begin(),hitTBArray.end(), [&x,&y,&hitQ](ATHit& hit){ TVector3 pos = hit.GetPosition();x+=pos.X()*hit.GetCharge();y+=pos.Y()*hit.GetCharge();hitQ+=hit.GetCharge();});
                  x/=hitQ;
                  y/=hitQ;
                  hitCluster->SetCharge(hitQ);
                  hitCluster->SetPosition(x,y,hitTBArray.at(0).GetPosition().Z());
                  hitCluster->SetTimeStamp(hitTBArray.at(0).GetTimeStamp());
                  TMatrixDSym cov(3); //TODO: Setting covariant matrix based on pad size and drift time resolution. Using estimations for the moment.
                  cov(0,1) = 0;
                  cov(1,2) = 0;
                  cov(2,0) = 0;
                  cov(0,0) = 0.04;
  	          cov(1,1) = 0.04;
  		  cov(2,2) = 0.01;
                  hitCluster->SetCovMatrix(cov);
                  ++clusterID;
                  track.AddClusterHit(hitCluster);
		  
                  
		}
   
          }

          //Sanity check
          /*std::vector<ATHitCluster>* hitClusterArray = track.GetHitClusterArray();
          std::cout<<" Clusterized hits : "<<hitClusterArray->size()<<"\n";
          for(auto iClusterHits=0;iClusterHits<hitClusterArray->size();++iClusterHits)
	  {
	    TVector3 pos    = hitClusterArray->at(iClusterHits).GetPosition();
            double clusterQ = hitClusterArray->at(iClusterHits).GetCharge();
            int TB          = hitClusterArray->at(iClusterHits).GetTimeStamp();
	    std::cout<<" Pos : "<<pos.X()<<" - "<<pos.Y()<<" - "<<pos.Z()<<" - TB : "<<TB<<" - Charge : "<<clusterQ<<"\n";	

	  }*/
        

}

Double_t fitf(Double_t *x,Double_t *par)
{

  return par[0]+par[1]*x[0];

}





