void runreco_sim
(TString mcFile = "output_digi.root",
 TString digiParFile = "/mnt/simulations/attpcroot/fair_install_2020/yassid/ATTPCROOTv2/parameters/ATTPC.e15250_GENFIT_sim.par",
 TString mapParFile = "/mnt/simulations/attpcroot/fair_install_2020/yassid/ATTPCROOTv2/scripts/scripts/Lookup20150611.xml",
 TString trigParFile = "/mnt/simulations/attpcroot/fair_install_2020/yassid/ATTPCROOTv2/parameters/AT.trigger.par")
{
  // -----   Timer   --------------------------------------------------------
 TStopwatch timer;
 timer.Start();


  TString scriptfile = "Lookup20150611.xml";
  TString dir = getenv("VMCWORKDIR");
  TString scriptdir = dir + "/scripts/"+ scriptfile;
  TString dataDir = dir + "/macro/data/";
  TString geomDir = dir + "/geometry/";
  gSystem -> Setenv("GEOMPATH", geomDir.Data());

 // ------------------------------------------------------------------------
  // __ Run ____________________________________________
  FairRunAna* fRun = new FairRunAna();
              fRun -> SetInputFile(mcFile);
              fRun -> SetGeomFile("/mnt/simulations/attpcroot/fair_install_2020/yassid/ATTPCROOTv2/geometry/ATTPC_H1bar_geomanager.root");
              fRun -> SetOutputFile("output_digi_PRA.root");


  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
              FairParAsciiFileIo* parIo1 = new FairParAsciiFileIo();
              parIo1 -> open(digiParFile.Data(), "in");
              rtdb -> setFirstInput(parIo1);
              FairParAsciiFileIo* parIo2 = new FairParAsciiFileIo();
              parIo2 -> open(trigParFile.Data(), "in");
              rtdb -> setSecondInput(parIo2);

  // __ AT digi tasks___________________________________

      ATPSATask *psaTask = new ATPSATask();
      psaTask -> SetPersistence(kTRUE);
      psaTask -> SetThreshold(10);
      //psaTask -> SetPSAMode(1); //NB: 1 is ATTPC - 2 is pATTPC
      psaTask -> SetPSAMode(1); //FULL mode
      //psaTask -> SetPeakFinder(); //NB: Use either peak finder of maximum finder but not both at the same time
      psaTask -> SetMaxFinder();
      psaTask -> SetBaseCorrection(kFALSE); //Directly apply the base line correction to the pulse amplitude to correct for the mesh induction. If false the correction is just saved
      psaTask -> SetTimeCorrection(kFALSE); //Interpolation around the maximum of the signal peak


      ATPRATask *praTask = new ATPRATask();
      praTask -> SetPersistence(kTRUE);

      ATFitterTask *fitterTask = new ATFitterTask();
      fitterTask -> SetPersistence(kTRUE);  

  fRun -> AddTask(psaTask);
  fRun -> AddTask(praTask);
  fRun -> AddTask(fitterTask);
 

  // __ Init and run ___________________________________

  fRun -> Init();
  fRun -> Run(0,100);

  std::cout << std::endl << std::endl;
  std::cout << "Macro finished succesfully."  << std::endl << std::endl;
  // -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  // ------------------------------------------------------------------------
}
