#include "genfit_Test.hh"

int main()
{   

   FairRunAna* run = new FairRunAna(); //Forcing a dummy run
   TString FileName = "/mnt/simulations/attpcroot/fair_install_2020/yassid/ATTPCROOTv2/macro/Simulation/ATTPC/12Be_pp/output_digi_PRA.root";
   std::cout<<" Opening File : "<<FileName.Data()<<std::endl;
   TFile* file = new TFile(FileName.Data(),"READ");
  
   TTree* tree = (TTree*) file -> Get("cbmsim");
   Int_t nEvents = tree -> GetEntries();
   std::cout<<" Number of events : "<<nEvents<<std::endl;

   TTreeReader Reader1("cbmsim", file);
   TTreeReaderValue<TClonesArray> eventArray(Reader1, "ATPatternEvent");
   TTreeReaderValue<std::vector<genfit::Track>> fitterVector(Reader1, "ATTPC");

   //GENFIT geometry
   new TGeoManager("Geometry", "ATTPC geometry");
   TGeoManager::Import("/mnt/simulations/attpcroot/fair_install_2020/yassid/ATTPCROOTv2/geometry/ATTPC_H1bar_geomanager.root");
   genfit::MaterialEffects::getInstance()->init(new genfit::TGeoMaterialInterface());
   genfit::FieldManager::getInstance()->init(new genfit::ConstField(0. ,0.,20.)); // 

   //Histograms
   TH1F* angle = new TH1F("angle","angle",720,0,179);
   TH1F* momentum = new TH1F("momentum","momentum",1000,0,2.0);//GeV
   TH2F* angle_vs_momentum = new TH2F("angle_vs_momentum","angle_vs_momentum",720,0,179,1000,0,2.0);
   TH2F* pos_vs_momentum = new TH2F("pos_vs_momentum","pos_vs_momentum",200,0,200,1000,0,2.0);
   TH2F* length_vs_momentum = new TH2F("length_vs_momentum","length_vs_momentum",200,0,200,1000,0,2.0);
   TH2F* hits_vs_momentum = new TH2F("hits_vs_momentum","hits_vs_momentum",200,0,200,1000,0,2.0);

   //event display
   genfit::EventDisplay* display = genfit::EventDisplay::getInstance();

   for(Int_t i=0;i<nEvents;i++){
         
              std::cout<<" Event Number : "<<i<<"\n";

              Reader1.Next();
              ATPatternEvent* patternEvent = (ATPatternEvent*) eventArray->At(0);
              
              auto numGenfitTracks = fitterVector->size();
	      std::cout<<" Number of tracks : "<<numGenfitTracks<<std::endl;
              

              for(auto iTrack=0;iTrack<numGenfitTracks;++iTrack)
              {

		TVector3 pos_res;
                TVector3 mom_res;
                TMatrixDSym cov_res;

               try{
                    auto genfitTrack = fitterVector->at(iTrack);
                    //genfitTrack.Print();
                    if(genfitTrack.hasKalmanFitStatus())
                    {
                        auto KalmanFitStatus = genfitTrack.getKalmanFitStatus();
                        
			if(KalmanFitStatus->isFitted())
                        {
		 		KalmanFitStatus->Print();
		                genfit::MeasuredStateOnPlane fitState = genfitTrack.getFittedState();
		                fitState.Print();
		                fitState.getPosMomCov(pos_res,mom_res,cov_res);              
		                display->addEvent(&genfitTrack);
		                angle->Fill(mom_res.Theta()*TMath::RadToDeg());
		                //std::cout<<" Angle "<<mom_res.Theta()<<"\n";
		                auto pos_radial = TMath::Sqrt(TMath::Power(pos_res.X(),2) + TMath::Power(pos_res.Y(),2));
		                momentum->Fill(mom_res.Mag());
		                angle_vs_momentum->Fill(mom_res.Theta()*TMath::RadToDeg(),mom_res.Mag());
		                pos_vs_momentum->Fill(pos_res.Mag(),mom_res.Mag());
		                auto len = genfitTrack.getTrackLen();
		                length_vs_momentum->Fill(len,mom_res.Mag());
		                auto numHits = genfitTrack.getNumPoints();
				hits_vs_momentum->Fill(numHits,mom_res.Mag());
   			}
	            }                    


               }catch(std::exception& e){
        	 std::cout<<" "<< e.what()<<"\n";
                 break;
  	       }

              }


	      std::vector<ATTrack>& patternTrackCand =  patternEvent->GetTrackCand();

    }

    TCanvas *c1 = new TCanvas();
    c1->Divide(2,2);
    c1->Draw();
    c1->cd(1);
    angle->Draw();
    c1->cd(2);
    momentum->Draw();
    c1->cd(3);
    angle_vs_momentum->Draw(); 
    c1->cd(4);
    pos_vs_momentum->Draw(); 

    TCanvas *c2 = new TCanvas();
    c2->Divide(2,2);
    c2->Draw();
    c2->cd(1);   
    length_vs_momentum->Draw();
    c2->cd(2);
    hits_vs_momentum->Draw();
  

    // open event display
    display->open();

}
