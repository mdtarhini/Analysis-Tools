/*
This is a generated macro. It can be run standalone but it is better to regenerate it for ech use-case sepaaretly. The macro is generated based on a config text provided for the macro "RunMyTask.C" in the Task-Submitter" directory. Refer to the README there.
*/
#if !defined(__CHANGETHISTOTHEMACRONAME__)
#define __CHANGETHISTOTHEMACRONAME__

//-------------------------------------------------------------------------------------------------//
//possible run modes
enum enumRunMode
{
  kRunLocal,
  kRunGridTest,
  kRunGridFull,
  kRunGridTerminate,
  kRunGridMerge
};
//-------------------------------------------------------------------------------------------------//


//-------------------------------------------------------------------------------------------------//
//Dummy function for library loading
void LoadLibraries()
{
  // Load common libraries
  gSystem->Load("libTree");
  gSystem->Load("libGeom");
  gSystem->Load("libPhysics");
  gSystem->Load("libVMC");
  gSystem->Load("libMinuit.so");
  gSystem->Load("libSTEERBase");
  gSystem->Load("libESD");
  gSystem->Load("libAOD");
  gSystem->Load("libANALYSIS");
  gSystem->Load("libOADB.so");
  gSystem->Load("libANALYSISalice");
  gSystem->Load("libEventMixing");
  gSystem->Load("libCORRFW");
  gSystem->Load("libPWGmuon");
  gSystem->Load("libANALYSISaliceBase");
  gSystem->AddIncludePath(Form("-I\"%s/include\"", gSystem->ExpandPathName("$ALICE_ROOT")));
  gROOT->ProcessLine(Form(".include %s/include", gSystem->ExpandPathName("$ALICE_ROOT")));
  gSystem->AddIncludePath(Form("-I\"%s/include\"", gSystem->ExpandPathName("$ALICE_PHYSICS")));
  gROOT->ProcessLine(Form(".include %s/include", gSystem->ExpandPathName("$ALICE_PHYSICS")));
}
//-------------------------------------------------------------------------------------------------//


//-------------------------------------------------------------------------------------------------//
// Create a chain usin a AOD root file. To be updated for ESD use
TChain *CreateChainFromAODFile(const char *rootfile)
{
  TChain *chain = 0;
  chain = new TChain("aodTree");
  chain->Add(rootfile);
  if (!chain->GetNtrees())
    return NULL;
  chain->ls();
  return chain;
}
//-------------------------------------------------------------------------------------------------//


//-------------------------------------------------------------------------------------------------//
// Create an alien handler for the different grid run modes
AliAnalysisGrid *CreateAlienHandler(TString inputFile, Int_t runOption)
{
  AliAnalysisAlien *plugin = new AliAnalysisAlien();
  plugin->SetOverwriteMode();

  //can be "full", "test", "offline", "submit" or "terminate"
  if (runOption == kRunGridTest)
  {
    plugin->SetRunMode("test");
    plugin->SetNtestFiles(1);
  }
  if (runOption == kRunGridFull)
  {
    plugin->SetRunMode("full");
    plugin->SetMergeViaJDL();
  }
  if (runOption == kRunGridTerminate)
  {
    plugin->SetRunMode("terminate");
    plugin->SetMergeViaJDL();
  }
  if (runOption == kRunGridMerge)
  {
    plugin->SetRunMode("terminate");
  }
  plugin->SetCheckCopy(kFALSE);
  plugin->AddIncludePath("-I. -I$ROOTSYS/include -I$ALICE_ROOT -I$ALICE_ROOT/include -I$ALICE_PHYSICS/include");
  plugin->SetAPIVersion("V1.1x");
  plugin->SetAliPhysicsVersion("CHANGETHISTOALIPHYICSVERSION");
  

  plugin->SetGridDataDir("CHANGETHISTOGRIDDATADIR");
  plugin->SetDataPattern("CHANGETHISTOGRIDDATAPATTERN");
  plugin->SetRunPrefix("CHANGETHISTORUNPREFIX");
  TString strRunList = gSystem->GetFromPipe(Form("cat %s", inputFile.Data()));
  plugin->AddRunList(strRunList.Data());
  plugin->SetOutputToRunNo(1); //to set the name of output folders as the run numbber
  plugin->SetGridWorkingDir("CHANGETHISTOGRIDWORKINGDIR");
  plugin->SetGridOutputDir("CHANGETHISTOGRIDOUTPUTDIR");


  plugin->SetAnalysisSource("CHANGETHISTOADDANALYSISSOURCE");
  plugin->SetAdditionalLibs("CHANGETHISTOADDITIONALLIBS");
  plugin->SetDefaultOutputs(kTRUE);
  plugin->SetAnalysisMacro("CHANGETHISTOTASKNAME.C");
  plugin->SetSplitMaxInputFileNumber(40);
  plugin->SetExecutable("CHANGETHISTOTASKNAME.sh");
  plugin->SetTTL(30000);
  plugin->SetJDLName("CHANGETHISTOTASKNAME.jdl");

  plugin->SetPrice(1);
  plugin->SetSplitMode("se");
  return plugin;
}
//-------------------------------------------------------------------------------------------------//


//-------------------------------------------------------------------------------------------------//
//The main task of the macro. Depending on the run mode it will call one of the above functions. Some code refinements is required for root6 but it should work just like that.
void RunTask(TString inputFileName = "/Users/tarhini/work/Analyses/test/runList_LHC17m.txt", Int_t runMode = kRunGridFull)
{
  Int_t nEvents = 10e6;
  // Load common libraries
  LoadLibraries();
  TChain *chain;
  AliAnalysisGrid *alienHandler;

  CHANGETHISTOTHECLASSLOADING

  if (runMode == kRunLocal)
  {
    chain = CreateChainFromAODFile(inputFileName);
    if (!chain)
      return;
  }
  else
  {
    alienHandler = CreateAlienHandler(inputFileName, runMode);
    if (!alienHandler)
      return;
  }

  gSystem->Exec(TString::Format("cp %s __runTask__.C", __FILE__));
  gROOT->LoadMacro("__runTask__.C");
  gSystem->Exec("rm __runTask__.C");
  gROOT->ProcessLineSync(TString::Format("CreateAnalysisTrain()"));

  AliAnalysisManager *mgr = AliAnalysisManager::GetAnalysisManager();
  mgr->SetDebugLevel(2);

  if (runMode == kRunLocal)
  {
    if (mgr->InitAnalysis())
    {
      mgr->StartAnalysis("local", chain, nEvents);
    }
  }
  else
  {
    mgr->SetGridHandler(alienHandler);
    if (mgr->InitAnalysis())
    {
      mgr->PrintStatus();
      mgr->StartAnalysis("grid");
    }
  }
}
//-------------------------------------------------------------------------------------------------//


#else
//-------------------------------------------------------------------------------------------------//
void CreateAnalysisTrain()
{
  /// create the analysis train and configure it

  // analysis manager
  AliAnalysisManager *mgr = new AliAnalysisManager();
  //mgr->SetDebugLevel(3);

  // AOD handler
  AliInputEventHandler *aodH = new AliAODInputHandler;
  mgr->SetInputEventHandler(aodH);

  CHANGETHISTOTHEADDCLASSLOADING

  return;
}
//-------------------------------------------------------------------------------------------------//
#endif
