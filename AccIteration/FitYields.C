/*
For a given iteration step, this macro calculate the yield of psi(2s) vs pt using the Acc*Eff values obtained from the previous step. It then fits the distribution and store the fit parameters to be used in the next step. As this analsyis is not too high on statistics, some parameter fine tuning could be necessary in some steps. The fit description is included in the class "PtDist.cxx".
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
#include "PtDist.h"
#include "PtDist.cxx"
// #include "Common.C"

using namespace std;
using namespace RooFit;

//--------------------------------------------------------------------------------------------//
TFile *filePsi2sNumbers = new TFile("NPsi2s.root");
TH1F *histoPsi2sNumbersVsPt = ((TH1F *)filePsi2sNumbers->Get("histoNpsi2sVsPt"));
TH1F *histoPsi2sNumbersVsRap = ((TH1F *)filePsi2sNumbers->Get("histoNpsi2sVsRapidity"));
//--------------------------------------------------------------------------------------------//

//--------------------------------------------------------------------------------------------//
void FitPtYield(Int_t iteration=1)
{

  //Get the corresponding Acc*Effi file
  TFile *inputAccEffFile = new TFile(Form("AccEffi/iter-%d/AccEffiValues.root",iteration-1));
  TH1F *histoAccEffiVsPt = ((TH1F *)inputAccEffFile->Get("histoAccEffiVsPt"));


  TH1F *histoYiledVsPt = new TH1F(*histoPsi2sNumbersVsPt);
  histoYiledVsPt->Divide(histoAccEffiVsPt);


  //--------------------------------------------------------------------------------------------//
  //Roofit setup
  RooRealVar pt("pt", "pt_{#mu #mu}", 0, 12, "GeV/c");
  RooPlot *frame = pt.frame(Title(Form("Iteration: Step %d",iteration)));
  RooDataHist ptData("ptData", "ptData", pt, Import(*histoYiledVsPt));
  ptData.plotOn(frame, Name("ptData"), MarkerSize(0.1));

  RooRealVar p1("p1", "p1", 1, -10, 10);
  RooRealVar p2("p2", "p2", 1, -10, 10);
  RooRealVar p3("p3", "p3", 1, -10, 10);

  PtDist ptShape("ptShape", "ptShape", pt, p1, p2, p3);

  RooFitResult *fitRes = ptShape.fitTo(ptData, Save(kTRUE));
  ptShape.plotOn(frame, Name("gaus"), LineColor(kBlue));
  
  TCanvas *canYieldPt = new TCanvas(Form("canYieldPt_%d",iteration), "", 1020, 800);
  SetCanvasStyle(canYieldPt);
  canYieldPt->SetLogy();

  frame->GetYaxis()->SetTitleOffset(1.4);
  frame->Draw("same");

  TLegend *legMain = new TLegend(0.235, 0.3, 0.455, 0.5);
  legMain->SetMargin(0.1);
  legMain->SetFillStyle(0);
  legMain->SetLineColorAlpha(0, 0);
  legMain->SetTextColor(kBlack);
  legMain->AddEntry("NULL", Form("Iteration: Step-%d", iteration), "");
  legMain->AddEntry("NULL", Form("p_{1} = %2.2f", p1.getVal()), "");
  legMain->AddEntry("NULL", Form("p_{2} = %2.2f", p2.getVal()), "");
  legMain->AddEntry("NULL", Form("p_{3} = %2.2f", p3.getVal()), "");
  legMain->Draw();
  //--------------------------------------------------------------------------------------------//


  //--------------------------------------------------------------------------------------------//
  //Store the values of the fit. Use 1 (arbitrary) for the normalisation parameter (1st) as a normalisation will be done in later steps.
  gSystem->Exec(Form("mkdir -p PtShapeIterations/iter-%d",iteration));
  canYieldPt->SaveAs(Form("PtShapeIterations/iter-%d/PtYield.pdf", iteration));
  ofstream outputFile(Form("PtShapeIterations/iter-%d/values.txt", iteration), std::ofstream::out);
  outputFile<<"1"<<endl;
  outputFile<<p1.getVal()<<endl;
  outputFile<<p2.getVal()<<endl;
  outputFile<<p3.getVal()<<endl;
  outputFile.close();
  //--------------------------------------------------------------------------------------------//

}
//--------------------------------------------------------------------------------------------//

/*
For a given iteration step, this macro calculate the yield of psi(2s) vs rapidity using the Acc*Eff values obtained from the previous step. It then fits the distribution and store the fit parameters to be used in the next step. The fit function is a 0-centered gaussian.
*/

//--------------------------------------------------------------------------------------------//
void FitRapYield(Int_t iteration = 1)
{

  //Get the corresponding Acc*Effi file
  TFile *inputAccEffFile = new TFile(Form("AccEffi/iter-%d/AccEffiValues.root", iteration - 1));
  TH1F *histoAccEffiVsRap = ((TH1F *)inputAccEffFile->Get("histoAccEffiVsRap"));

  TH1F *histoYiledVsRapidity = new TH1F(*histoPsi2sNumbersVsRap);
  histoYiledVsRapidity->Divide(histoAccEffiVsRap);

  //--------------------------------------------------------------------------------------------//
  //Roofit setup
  RooRealVar rapidity("rapidity", "y_{#mu #mu}", -4, -2.5, "");
  RooPlot *frame = rapidity.frame(Title("ddd"));
  RooDataHist rapidityData("rapidityData", "rapidityData", rapidity, Import(*histoYiledVsRapidity));
  rapidityData.plotOn(frame, Name("rapidityData"), MarkerSize(0.1));

  RooRealVar sigma("sigma", "sigma", -3, -10, 10);
  RooRealVar mean("mean", "mean", 0, -10, 10);
  mean.setConstant(kTRUE);
  RooGaussian gauss("gauss", "gauss", rapidity, mean, sigma);

  RooFitResult *fitRes = gauss.chi2FitTo(rapidityData, Save(kTRUE));
  gauss.plotOn(frame, Name("gaus"), LineColor(kBlue));

  TCanvas *canYieldRap = new TCanvas(Form("canYieldRap_%d",iteration), "", 1020, 800);
  SetCanvasStyle(canYieldRap);

  frame->GetYaxis()->SetTitleOffset(1.4);
  frame->Draw("same");

  TLegend *legMain = new TLegend(0.235, 0.745, 0.455, 0.88);
  legMain->SetMargin(0.1);
  legMain->SetFillStyle(0);
  legMain->SetLineColorAlpha(0, 0);
  legMain->SetTextColor(kBlack);
  legMain->AddEntry("NULL", Form("Iteration: Step-%d", iteration), "");
  legMain->AddEntry("NULL", Form("#sigma = %2.2f", sigma.getVal()), "");
  legMain->Draw();
  //--------------------------------------------------------------------------------------------//

  //--------------------------------------------------------------------------------------------//
  //Store the values of the fit. Use 1 (arbitrary) for the normalisation parameter (1st) as a normalisation will be done in later steps.
  gSystem->Exec(Form("mkdir -p RapShapeIterations/iter-%d", iteration));
  canYieldRap->SaveAs(Form("RapShapeIterations/iter-%d/RapYield.pdf", iteration));
  ofstream outputFile(Form("RapShapeIterations/iter-%d/values.txt", iteration), std::ofstream::out);
  outputFile << "1" << endl;
  outputFile << mean.getVal() << endl;
  outputFile << sigma.getVal() << endl;
  outputFile.close();
}
//--------------------------------------------------------------------------------------------//