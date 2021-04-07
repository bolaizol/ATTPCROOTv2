void analysis()
{
   gSystem->Load("libgenfit2.so");
   gSystem->AddIncludePath("/mnt/simulations/attpcroot/fair_install_2020/GenFit_Inst/include");

   FairRunAna* run = new FairRunAna(); //Forcing a dummy run
   TString FileName = "output_digi_PRA.root";
   std::cout<<" Opening File : "<<FileName.Data()<<std::endl;
   TFile* file = new TFile(FileName.Data(),"READ");
  
   TTree* tree = (TTree*) file -> Get("cbmsim");
   Int_t nEvents = tree -> GetEntries();
   std::cout<<" Number of events : "<<nEvents<<std::endl;

   TTreeReader Reader1("cbmsim", file);
   TTreeReaderValue<TClonesArray> eventArray(Reader1, "ATPatternEvent");
   //TTreeReaderValue<TClonesArray> fitterArray(Reader1, "genfit::Track");
   TTreeReaderValue<std::vector<genfit::Track>> fitterVector(Reader1, "ATTPC");

   //GENFIT geometry
   new TGeoManager("Geometry", "ATTPC geometry");
   TGeoManager::Import("/mnt/simulations/attpcroot/fair_install_2020/yassid/ATTPCROOTv2/geometry/ATTPC_H1bar_geomanager.root");
   //genfit::MaterialEffects::getInstance()->init(new genfit::TGeoMaterialInterface());
   genfit::FieldManager::getInstance()->init(new genfit::ConstField(0. ,0.,20.)); // 

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

               try{
                    auto test = fitterVector->at(iTrack);

                     std::cout<<" Adding Track "<<"\n";
	             
		     //genfit::MeasuredStateOnPlane fitState = test.getFittedState();
                     display->addEvent(&test);
                    


               }catch(std::exception& e){
        	 std::cout<<" "<< e.what()<<"\n";
                 break;
  	       }

              }


	      std::vector<ATTrack>& patternTrackCand =  patternEvent->GetTrackCand();

                    

		
        }

    // open event display
    display->open();


}
