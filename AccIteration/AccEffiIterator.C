/*
This is the main macro which calls the ither functions for each iteration step. it can takes two arguments, the iteration step to start with and the one to stop at. To do only one itertaion step (e.g step 1) run root -l AccEffiIterator.C\(1,1\).
To do multiple steps (e.g from 0 to 3), run root -l AccEffiIterator.C\(0,3\). It is better to the steps one by one to make sure that the fits converge for each step.
*/

using namespace std;

void AccEffiIterator(int iterationStart = 0, int iterationEnd = 1)
{
  //-------------------------------------------------------------------//
  //Don't change the order of loading the following macros due to some conflictions caused by compiling RooFit
  gROOT->LoadMacro("FillHistoAndGetAccEffi.C+");
  gROOT->LoadMacro("FitYields.C");
  gROOT->LoadMacro("CompareEffi.C");
  //-------------------------------------------------------------------//

  for (int iteration = iterationStart; iteration <= iterationEnd; iteration++)
  {

    //-------------------------------------------------------------------//
    //Check if the necessary files from previous steps are available
    if (iteration > 0)
    {
      //Check the acceptance values
      if (gSystem->AccessPathName(Form("AccEffi/iter-%d/AccEffiValues.root", iteration - 1)))
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
      gROOT->ProcessLineSync(Form("FillHistoAndGetAccEffi(%d)", iteration));
      //In case of step=0, it will compare the Acc*Effi histos to themselves resulsting in weird illustartions. TODO: Fix CompareEffi.C
      gROOT->ProcessLineSync(Form("CompareEffi(%d,%d)", iteration, iteration));
    }

    else
    {
      //Fit the Pt and y shapes
      gROOT->ProcessLineSync(Form("FitPtYield(%d)", iteration));
      gROOT->ProcessLineSync(Form("FitRapYield(%d)", iteration));
      //Weight and get the Acc*Effi
      gROOT->ProcessLineSync(Form("FillHistoAndGetAccEffi(%d)", iteration));
      //Compare the Acc*Effi values of this step to the previous one
      gROOT->ProcessLineSync(Form("CompareEffi(%d,%d)", iteration - 1, iteration));
    }
  }

  return;
}