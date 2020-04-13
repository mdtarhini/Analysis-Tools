/*
For a given iteration step, this macro calculate the yield of J/psi vs pt using the Acc*Eff values obtained from the previous step. It then fits the distribution and store the fit parameters to be used in the next step. 
*/

#ifndef __CINT__
#include "RooGlobalFunc.h"
#endif
#include "TMath.h"
#include "TString.h"
#include "TFile.h"
#include "TF1.h"
#include "TH2F.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TROOT.h"
#include "TApplication.h"
#include "TFrame.h"
#include "TLegend.h"
#include "TObjArray.h"
#include "TGraphErrors.h"
#include "TFitResultPtr.h"
#include "TFitResult.h"
#include "TMatrixDSym.h"
#include "Math/MinimizerOptions.h"
#include "TDirectoryFile.h"
#include "RooRealVar.h"
#include "RooDataSet.h"
#include "RooDataHist.h"
#include "RooGaussian.h"
#include "RooFitResult.h"
#include "RooAddPdf.h"
#include "RooExtendPdf.h"
#include "RooPlot.h"
#include "RooProduct.h"
#include "RooArgList.h"

using namespace std;
using namespace RooFit;

//--------------------------------------------------------------------------------------------//
TFile *inputRawYieldFileVsRap = new TFile("RapRawYields.root");
TFile *inputRawYieldFileVsPt = new TFile("PtRawYields.root");


//--------------------------------------------------------------------------------------------//

//--------------------------------------------------------------------------------------------//
void FitPtYield(int multMin = 1, int multMax =8, Int_t iteration=1)
{
  TCanvas *canYieldPt = new TCanvas(Form("canYieldPt_%d",iteration), "", 1020, 800);
  SetCanvasStyle(canYieldPt);
  canYieldPt->SetLogy();

  //Get the corresponding Acc*Effi file
  TFile *inputAccEffFile = new TFile(Form("Mult-%dto%d/AccEffi/iter-%d/AccEffiValues.root",multMin,multMax,iteration-1));
  TH1F *histoAccEffiVsPt = ((TH1F *)inputAccEffFile->Get("histoAccEffiVsPt"));

  TH1F *histoJpsiNumbersVsPt = ((TH1F *)inputRawYieldFileVsPt->Get(Form("histoPtYield_mult%dto%d",multMin,multMax)));
  TH1F *histoYiledVsPt = new TH1F(*histoJpsiNumbersVsPt);
  histoYiledVsPt->Divide(histoAccEffiVsPt);

  
  TF1* fPtDistribution = new TF1("fPtDistribution","[0]*x/((1+(x/[1])**[2])**[3] )",0,20);
  fPtDistribution->SetParameter(0,histoYiledVsPt->GetMaximum());
  fPtDistribution->SetParameter(1,4);
  fPtDistribution->SetParameter(2,2);
  fPtDistribution->SetParameter(3,3);
  TFitResultPtr r = histoYiledVsPt->Fit(fPtDistribution,"REIS","",0,20);
  
  

  histoYiledVsPt->GetYaxis()->SetTitleOffset(1.4);
  TLegend *legMain = new TLegend(0.235, 0.3, 0.455, 0.5);
  legMain->SetMargin(0.1);
  legMain->SetFillStyle(0);
  legMain->SetLineColorAlpha(0, 0);
  legMain->SetTextColor(kBlack);
  legMain->AddEntry("NULL", Form("Iteration: Step-%d", iteration), "");
  legMain->AddEntry("NULL", Form("p_{1} = %2.2f", fPtDistribution->GetParameter(1)), "");
  legMain->AddEntry("NULL", Form("p_{2} = %2.2f", fPtDistribution->GetParameter(2)), "");
  legMain->AddEntry("NULL", Form("p_{3} = %2.2f", fPtDistribution->GetParameter(3)), "");
  legMain->Draw();
  //--------------------------------------------------------------------------------------------//


  //--------------------------------------------------------------------------------------------//
  //Store the values of the fit. Use 1 (arbitrary) for the normalisation parameter (1st) as a normalisation will be done in later steps.
  gSystem->Exec(Form("mkdir -p Mult-%dto%d/PtShapeIterations/iter-%d",multMin,multMax,iteration));
  canYieldPt->SaveAs(Form("Mult-%dto%d/PtShapeIterations/iter-%d/PtYield.pdf", multMin,multMax,iteration));
  ofstream outputFile(Form("Mult-%dto%d/PtShapeIterations/iter-%d/values.txt", multMin,multMax,iteration), std::ofstream::out);
  outputFile<<"1"<<endl;
  outputFile<<fPtDistribution->GetParameter(1)<<endl;
  outputFile<<fPtDistribution->GetParameter(2)<<endl;
  outputFile<<fPtDistribution->GetParameter(3)<<endl;
  outputFile.close();
  //--------------------------------------------------------------------------------------------//

}
//--------------------------------------------------------------------------------------------//

/*
For a given iteration step, this macro calculate the yield of j/psi vs rapidity using the Acc*Eff values obtained from the previous step. It then fits the distribution and store the fit parameters to be used in the next step. The fit function is a 0-centered gaussian.
*/

//--------------------------------------------------------------------------------------------//
void FitRapYield(int multMin = 1, int multMax =8, Int_t iteration = 1)
{

  TCanvas *canYieldRap = new TCanvas(Form("canYieldRap_%d",iteration), "", 1020, 800);
  SetCanvasStyle(canYieldRap);

  //Get the corresponding Acc*Effi file
  TFile *inputAccEffFile = new TFile(Form("Mult-%dto%d/AccEffi/iter-%d/AccEffiValues.root",multMin,multMax, iteration - 1));
  TH1F *histoAccEffiVsRap = ((TH1F *)inputAccEffFile->Get("histoAccEffiVsRap"));

  TH1F *histoJpsiNumbersVsRap = ((TH1F *)inputRawYieldFileVsRap->Get(Form("histoRapYield_mult%dto%d",multMin,multMax)));
  TH1F *histoYiledVsRapidity = new TH1F(*histoJpsiNumbersVsRap);
  histoYiledVsRapidity->Divide(histoAccEffiVsRap);

   histoYiledVsRapidity->GetYaxis()->SetTitleOffset(1.4);


  TF1* fRapDistribution = new TF1("fRapDistribution","[0]*exp(-0.5*(x/[1])**2)",-4, -2.5);
  fRapDistribution->SetParameter(0,histoYiledVsRapidity->GetMaximum());
  fRapDistribution->SetParameter(1,3);
  TFitResultPtr r = histoYiledVsRapidity->Fit(fRapDistribution,"REIS","",-4, -2.5);


  TLegend *legMain = new TLegend(0.235, 0.745, 0.455, 0.88);
  legMain->SetMargin(0.1);
  legMain->SetFillStyle(0);
  legMain->SetLineColorAlpha(0, 0);
  legMain->SetTextColor(kBlack);
  legMain->AddEntry("NULL", Form("Iteration: Step-%d", iteration), "");
  legMain->AddEntry("NULL", Form("#sigma = %2.2f", fRapDistribution->GetParameter(1)), "");
  legMain->Draw();
  //--------------------------------------------------------------------------------------------//

  //--------------------------------------------------------------------------------------------//
  //Store the values of the fit. Use 1 (arbitrary) for the normalisation parameter (1st) as a normalisation will be done in later steps.
  gSystem->Exec(Form("mkdir -p Mult-%dto%d/RapShapeIterations/iter-%d", multMin,multMax,iteration));
  canYieldRap->SaveAs(Form("Mult-%dto%d/RapShapeIterations/iter-%d/RapYield.pdf", multMin,multMax,iteration));
  ofstream outputFile(Form("Mult-%dto%d/RapShapeIterations/iter-%d/values.txt", multMin,multMax,iteration), std::ofstream::out);
  outputFile << "1" << endl;
  outputFile << "0" << endl;
  outputFile << fRapDistribution->GetParameter(1) << endl;
  outputFile.close();
}
//--------------------------------------------------------------------------------------------//