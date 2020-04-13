/*
This is the main macro which calls the ither functions for each iteration step and for a given multiplicity bin. it takes three arguments, the iteration step to start with and the one to stop at. To do only one itertaion step (e.g step 1) run root -l AccEffiIterator.C\(1,1\).
To do multiple steps (e.g from 0 to 3), run root -l AccEffiIterator.C\(0,3\). It is better to the steps one by one to make sure that the fits converge for each step. It also takes as an argument the multiplicity bin. It is not recommended to make a loop running over the various multiplicity bins in order to check manually the results and quality for each bin when it is done.
*/

using namespace std;
Int_t multRanges[][2] = {{0, 100}, {1, 8}, {9, 14}, {15, 20}, {21, 25}, {26, 33}, {34, 41}, {42, 50}, {51, 60}, {61, 100}};
int numberOfMultRanges = sizeof(multRanges) / sizeof(multRanges[0]);

void AccEffiIterator(int multBin = 1, int iterationStart = 4, int iterationEnd = 4)
{
  //-------------------------------------------------------------------//
  //Don't change the order of loading the following macros due to some conflictions caused by compiling RooFit
  gROOT->LoadMacro("FillHistoAndGetAccEffi.C+");
  gROOT->LoadMacro("FitYields.C");
  gROOT->LoadMacro("CompareIterationsEffi.C");
  //-------------------------------------------------------------------//

  if (iterationStart == 0)
  {
    gSystem->Exec(Form("mkdir -p Mult-%dto%d/AccEffi", multRanges[multBin][0], multRanges[multBin][1]));
    gSystem->Exec(Form("mkdir -p Mult-%dto%d/PtShapeIterations/iter-0", multRanges[multBin][0], multRanges[multBin][1]));
    gSystem->Exec(Form("mkdir -p Mult-%dto%d/RapShapeIterations/iter-0", multRanges[multBin][0], multRanges[multBin][1]));

    //copy the default mc inputs
    gSystem->Exec(Form("cp MC-Default-Inputs/Pt/values.txt Mult-%dto%d/PtShapeIterations/iter-0/.", multRanges[multBin][0], multRanges[multBin][1]));
    gSystem->Exec(Form("cp MC-Default-Inputs/Rap/values.txt Mult-%dto%d/RapShapeIterations/iter-0/.", multRanges[multBin][0], multRanges[multBin][1]));
  }

  for (int iteration = iterationStart; iteration <= iterationEnd; iteration++)
  {

    //-------------------------------------------------------------------//
    //Check if the necessary files from previous steps are available
    if (iteration > 0)
    {
      //Check the acceptance values
      if (gSystem->AccessPathName(Form("Mult-%dto%d/AccEffi/iter-%d/AccEffiValues.root", multRanges[multBin][0], multRanges[multBin][1], iteration - 1)))
      {
        cout << Form("AccEffi values from previous iteration step (%d) are not available. Execute root -l AccEffiIterator.C\\(%d\\)", iteration - 1, iteration - 1) << endl;
        return;
      }
    }
    //-------------------------------------------------------------------//

    //-------------------------------------------------------------------//
    //Run the different functions in order. TODO: MAke the code more flexible by letting the user chose to perform a single functions
    //If the iteration step is the first (0) then start by getting the acc*Effi values. Otherwise, start  by fitting the functions before calculating the weighted Acc*Effi

    if (iteration == 0)
    {
      gROOT->ProcessLineSync(Form("FillHistoAndGetAccEffi(%d,%d,%d)", multRanges[multBin][0], multRanges[multBin][1], iteration));
      //In case of step=0, it will compare the Acc*Effi histos to themselves resulsting in weird illustartions. TODO: Fix CompareIterationsEffi.C
      gROOT->ProcessLineSync(Form("CompareIterationsEffi(%d,%d,%d,%d)", multRanges[multBin][0], multRanges[multBin][1], iteration, iteration));
    }

    else
    {
      //Fit the Pt and y shapes
      gROOT->ProcessLineSync(Form("FitPtYield(%d,%d,%d)", multRanges[multBin][0], multRanges[multBin][1], iteration));
      gROOT->ProcessLineSync(Form("FitRapYield(%d,%d,%d)", multRanges[multBin][0], multRanges[multBin][1], iteration));
      //Weight and get the Acc*Effi
      gROOT->ProcessLineSync(Form("FillHistoAndGetAccEffi(%d,%d,%d)", multRanges[multBin][0], multRanges[multBin][1], iteration));
      //Compare the Acc*Effi values of this step to the previous one
      gROOT->ProcessLineSync(Form("CompareIterationsEffi(%d,%d,%d,%d)", multRanges[multBin][0], multRanges[multBin][1], iteration - 1, iteration));
    }
  }

  return;
}