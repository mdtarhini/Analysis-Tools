AliAnalysisGrid *CreateAlienHandler(TString runOption = "test")
{

  AliAnalysisAlien *plugin = new AliAnalysisAlien();

  plugin->SetOverwriteMode();

  //can be "full", "test", "offline", "submit" or "terminate"
  if (runOption == "test")
  {
    plugin->SetRunMode("test");
    plugin->SetNtestFiles(1);
  }
  if (runOption == "full")
  {
    plugin->SetRunMode("full");
       plugin->SetMergeViaJDL();
  }
  if (runOption == "terminate")
  {
    plugin->SetRunMode("terminate");
     plugin->SetMergeViaJDL();
  }
  if (runOption == "merge")
  {
    plugin->SetRunMode("terminate");
  }
  plugin->SetCheckCopy(kFALSE);
  // Set versions of used packages
  plugin->AddIncludePath("-I. -I$ROOTSYS/include -I$ALICE_ROOT -I$ALICE_ROOT/include -I$ALICE_PHYSICS/include");
  plugin->SetAPIVersion("V1.1x");
  plugin->SetAliPhysicsVersion("vAN-20181106-1");
  plugin->AddIncludePath("-I. -I$ALIEN_ROOT/api/lib -I$ROOTSYS/lib -I$ROOTSYS/include -I$ALICE_ROOT/include -I$ALICE_PHYSICS/include");
  plugin->SetGridDataDir(" /alice/sim/2020/LHC20a4");
  plugin->SetDataPattern("/AOD/*/AliAOD.Muons.root");

  // plugin->AddRunNumber(282441);

  TString strRunList = gSystem->GetFromPipe("cat runList.txt");
  plugin->AddRunList(strRunList.Data());

  plugin->SetOutputToRunNo(1); //to set the name of output folders as the run numbber
  plugin->SetGridWorkingDir("MC13TeVPsi2S");
  plugin->SetGridOutputDir("output");
  plugin->SetAnalysisSource("TaskCharmoniaDimuonMC.cxx");
  plugin->SetAdditionalLibs("TaskCharmoniaDimuonMC.h TaskCharmoniaDimuonMC.cxx");

  plugin->SetDefaultOutputs(kTRUE);
  plugin->SetAnalysisMacro("TaskCharmoniaDimuonMC.C");
  plugin->SetSplitMaxInputFileNumber(40);
  plugin->SetExecutable("TaskCharmoniaDimuonMC.sh");
  plugin->SetTTL(30000);
  //  plugin->SetInputFormat("xml-single");

  plugin->SetJDLName("TaskCharmoniaDimuonMC.jdl");

  plugin->SetPrice(1);
  plugin->SetSplitMode("se");
  return plugin;
}

void runGrid(TString runOption = "merge")
{
  // Load common libraries
  gSystem->Load("libCore.so");
  gSystem->Load("libTree.so");
  gSystem->Load("libGeom.so");
  gSystem->Load("libVMC.so");
  gSystem->Load("libPhysics.so");
  gSystem->Load("libSTEERBase");
  gSystem->Load("libESD");
  gSystem->Load("libAOD");
  gSystem->Load("libANALYSIS");
  gSystem->Load("libANALYSISalice");
  // Use AliRoot includes to compile our task
  gROOT->ProcessLine(".include $ALICE_ROOT/include");
  gROOT->ProcessLine(".include $ALICE_PHYSICS/include");

  // Create and configure the alien handler plugin
  //gROOT->LoadMacro("CreateAlienHandler.C");
  AliAnalysisGrid *alienHandler = CreateAlienHandler(runOption);
  if (!alienHandler)
    return;

  // Create the analysis manager
  AliAnalysisManager *mgr = new AliAnalysisManager("testAnalysis");

  // Connect plug-in to the analysis manager
  mgr->SetGridHandler(alienHandler);

  AliAODInputHandler *aodH = new AliAODInputHandler();
  mgr->SetInputEventHandler(aodH);

  // TaskCharmoniaDimuonMC *task = new TaskCharmoniaDimuonMC("TaskCharmoniaDimuonMC",254400,264400);
  // mgr->AddTask(task);
  //  mgr->AddTask(taskPS);

  gROOT->LoadMacro("TaskCharmoniaDimuonMC.cxx++g");
  AliAnalysisTask *task = new TaskCharmoniaDimuonMC("TaskCharmoniaDimuonMC",254400,295000);
  mgr->AddTask(task);

  // Create containers for input/output
  AliAnalysisDataContainer *cinput = mgr->GetCommonInputContainer();
  AliAnalysisDataContainer *coutput = mgr->CreateContainer("ListEvent", TObjArray::Class(), AliAnalysisManager::kOutputContainer, "AnalysisResults.root");

  // Connect input/output
  mgr->ConnectInput(task, 0, cinput);
  mgr->ConnectOutput(task, 1, coutput);

  // Enable debug printouts
  mgr->SetDebugLevel(2);

  if (!mgr->InitAnalysis())
    return;

  mgr->PrintStatus();
  // Start analysis in grid.
  mgr->StartAnalysis("grid");
}
