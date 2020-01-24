/*
A tentative to estimate the systematic due to the pt-y correlation on the psi(2s) MC input using the Jpsi ones. This macro fits the cross section ratios vs pt and y with a pol1 function and generate other functions by varying the fits parameters within their uncertainties.
root -l FitCharmoniaRatio.C
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
#include "TMinuit.h"
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
#include "../Common.C"

using namespace std;
using namespace RooFit;

//The number of functions to be generated for each fit
int numberOfTrials = 50;

Double_t arrayRapBins[] = {-4, -3.75, -3.5, -3.25, -3, -2.75, -2.5};
int numberOfRapBins = sizeof(arrayRapBins) / sizeof(arrayRapBins[0]) - 1;
Double_t arrayRatioVsRap[] = {0.1612, 0.1382, 0.1608, 0.1377, 0.1546, 0.1531};
Double_t arrayRatioVsRap_statErr[] = {0.0385, 0.0215, 0.0191, 0.0176, 0.0209, 0.0383};
Double_t arrayRatioVsRap_sysErr[] = {0.0187, 0.0103, 0.0081, 0.0085, 0.0140, 0.0192};

Double_t arrayPtBins[] = {0, 1, 2, 3, 4, 5, 6, 8, 10, 12};
int numberOfPtBins = sizeof(arrayPtBins) / sizeof(arrayPtBins[0]) - 1;
Double_t arrayRatioVsPt[] = {0.0817, 0.1257, 0.1992, 0.1205, 0.2208, 0.1778, 0.2058, 0.2288, 0.3877};
Double_t arrayRatioVsPt_statErr[] = {0.0215, 0.0166, 0.0209, 0.0257, 0.0360, 0.0436, 0.0577, 0.0796, 0.0827};
Double_t arrayRatioVsPt_sysErr[] = {0.0115, 0.0092, 0.0105, 0.0106, 0.0230, 0.0135, 0.0194, 0.0283, 0.0275};

//--------------------------------------------------------------------------------------------//
void FitRatioVsRap()
{

  TCanvas *canRatioVsRap = new TCanvas("canRatioVsRap", "", 1020, 800);
  SetCanvasStyle(canRatioVsRap);

  TH1F *histoRatioVsRap = new TH1F("histoRatioVsRap", "", numberOfRapBins, &arrayRapBins[0]);
  histoRatioVsRap->Sumw2();
  for (int iRap = 0; iRap < numberOfRapBins; iRap++)
  {
    histoRatioVsRap->SetBinContent(iRap + 1, arrayRatioVsRap[iRap]);
    Double_t statError = arrayRatioVsRap_statErr[iRap] / arrayRatioVsRap[iRap];
    Double_t sysError = arrayRatioVsRap_sysErr[iRap] / arrayRatioVsRap[iRap];
    Double_t error = TMath::Sqrt(statError * statError + sysError * sysError) * arrayRatioVsRap[iRap];
    histoRatioVsRap->SetBinError(iRap + 1, error);
  }
  histoRatioVsRap->SetMinimum(0);
  histoRatioVsRap->SetMaximum(0.3);

  TF1 *fitRatioVsRap = new TF1("fitRatioVsRap", "[0]*x+[1]", -4, -2.5);
  histoRatioVsRap->Fit(fitRatioVsRap, "S");

  //Get the countour of parameter correlations within two sigmas
  gMinuit->SetErrorDef(4);
  TGraph *grCountourVsRap = (TGraph *)gMinuit->Contour(numberOfTrials, 0, 1);

  Double_t a, b;
  TFile *outputFile = new TFile("RapidityShapes.root", "recreate");
  for (int iPoint = 0; iPoint < numberOfTrials; iPoint++)
  {
    TF1 *fitSmeared = new TF1(Form("RatioVsRap_%d", iPoint), "[0]*x+[1]", -4, -2.5);
    grCountourVsRap->GetPoint(iPoint, a, b);
    fitSmeared->FixParameter(0, a);
    fitSmeared->FixParameter(1, b);
    // fitSmeared->SetLineColor(iPoint);
    fitSmeared->SetLineColorAlpha(iPoint, 0.25);
    fitSmeared->DrawCopy("same");
    fitSmeared->Write();
  }

  histoRatioVsRap->Draw("same");
  canRatioVsRap->Write();
  canRatioVsRap->SaveAs("CharmoniaRatioVsRap.pdf");
  delete outputFile;
}

//--------------------------------------------------------------------------------------------//
void FitRatioVsPt()
{

  TCanvas *canRatioVsPt = new TCanvas("canRatioVsPt", "", 1020, 800);
  SetCanvasStyle(canRatioVsPt);

  TH1F *histoRatioVsPt = new TH1F("histoRatioVsPt", "", numberOfPtBins, &arrayPtBins[0]);
  histoRatioVsPt->Sumw2();
  for (int iPt = 0; iPt < numberOfPtBins; iPt++)
  {
    histoRatioVsPt->SetBinContent(iPt + 1, arrayRatioVsPt[iPt]);
    Double_t statError = arrayRatioVsPt_statErr[iPt] / arrayRatioVsPt[iPt];
    Double_t sysError = arrayRatioVsPt_sysErr[iPt] / arrayRatioVsPt[iPt];
    Double_t error = TMath::Sqrt(statError * statError + sysError * sysError) * arrayRatioVsPt[iPt];
    histoRatioVsPt->SetBinError(iPt + 1, error);
  }
  histoRatioVsPt->SetMinimum(0);
  histoRatioVsPt->SetMaximum(0.8);

  TF1 *fitRatioVsPt = new TF1("fitRatioVsPt", "[0]*x+[1]", 0, 12);
  histoRatioVsPt->Fit(fitRatioVsPt, "S");

  //Get the countour of parameter correlations within two sigmas
  gMinuit->SetErrorDef(4);
  TGraph *grCountourVsPt = (TGraph *)gMinuit->Contour(numberOfTrials, 0, 1);

  Double_t a, b;
  TFile *outputFile = new TFile("PtShapes.root", "recreate");
  for (int iPoint = 0; iPoint < numberOfTrials; iPoint++)
  {
    TF1 *fitSmeared = new TF1(Form("RatioVsPt_%d", iPoint), "[0]*x+[1]", 0, 12);
    grCountourVsPt->GetPoint(iPoint, a, b);
    fitSmeared->FixParameter(0, a);
    fitSmeared->FixParameter(1, b);
    // fitSmeared->SetLineColor(iPoint);
    fitSmeared->SetLineColorAlpha(iPoint, 0.25);
    fitSmeared->DrawCopy("same");
    fitSmeared->Write();
  }

  histoRatioVsPt->Draw("same");
  canRatioVsPt->Write();
  canRatioVsPt->SaveAs("CharmoniaRatioVsPt.pdf");
  delete outputFile;
}
//--------------------------------------------------------------------------------------------//

void FitCharmoniaRatio()
{
  FitRatioVsRap();
  FitRatioVsPt();
}
//--------------------------------------------------------------------------------------------//