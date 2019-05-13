/*
In this macro, define the common parameters, such as the signal and back functions index and names.
*/

#include "TCanvas.h"
#include "TMath.h"
#include "TH1.h"
#include "TH2.h"
#include "TFile.h"
#include "TStyle.h"
#include "TROOT.h"
#include "TLegend.h"

enum cbTailsParams {kCBAlpha,kCBn,kCBAlphaPrime,kCBnPrime};
enum vectorFitResult {kNjpsi,KNJpsi_err,kJpsiMeanPos,kJpsiMeanPos_err,kJpsiSigma,kJpsiSigma_err, kChi2OverNfd};
const int numberOfVariables = 5;
enum FitResultVariables {kNJpsiPerTest,kJpsiMeanPerTest,kJpsiSigmaPerTest,kChi2PerTest,kTestFailure};
const char *variableNames[] = {"JpsiNumber","JpsiMean","JpsiSigma","ChiSquare","testFailure"};
enum enumFunctions{kCB21S,kNA601S,kCB22S,kNA602S,kCB23S,kNA603S,kVWG,kVWG2,kPol1OverPol2,kPol2OverPol3,kExpo,kDoubleExpo,kExpoPol2};
const TString arrayFunctionNames[] = {"CB2","NA60","CB2","NA60","CB2","NA60","VWG","VWG2","Pol1ToPol2","Pol2ToPol3","expo","2*expo","expo*Pol2"};
const Int_t totalNumberOfTails = 8;
// const Double_t arrayDefaultTails[totalNumberOfTails] = {0.982, 2.092, 2.039, 2.469, 0, 0, 0, 0};
const Double_t arrayDefaultTails[totalNumberOfTails] = {0.970014, 3.9789, 2.29866, 3.0301, 0, 0, 0, 0};



int IndexInVector(Double_t element, std::vector<double> vector,int vectorSize){
  for(int i=0;i<=vectorSize;i++){
    if(element == vector[i])
    return i;
  }
  return -1;
}
Bool_t IsInArray(int value, const int *array, int arraySize){
  for(int iElement = 0; iElement < arraySize; iElement++){
    if(value == array[iElement]){
      return kTRUE;
    }
  }
  return kFALSE;
}
Bool_t IsValueAlreadyInVector(std::vector<double> vector, Double_t value){
  for(int iElement = 0; iElement < vector.size(); iElement++){
    if(value == vector[iElement]){
      return kTRUE;
    }
  }
  return kFALSE;
}
std::vector<double> StringToVector(TString strBins = "0,90"){
  std::vector<double> vectorBins;
  TObjArray *objBins = strBins.Tokenize(",");
  TIter nextBin(objBins);
  TObjString *strBin;
  while ((strBin=(TObjString*)nextBin())) {
    TString currentBin = strBin->GetName();
    Double_t bin = currentBin.Atof();
    vectorBins.push_back(bin);
  }
  return vectorBins;
}
void SetCanvasStyle(TCanvas *can){
  gStyle->SetOptStat(0);
  // gStyle->SetOptTitle(0);
  int font = 42;
  // gROOT->SetStyle("Plain");
  gStyle->SetTitleFillColor(0);
  gStyle->SetTitleBorderSize(0);
  // gStyle->SetTitleAlign(12);
  gStyle->SetLegendBorderSize(1);
  gStyle->SetTextFont(font);
  gStyle->SetTickLength(0.02,"y");
  gStyle->SetEndErrorSize(3);
  gStyle->SetLabelSize(0.04,"xyz");
  gStyle->SetLabelFont(font,"xyz");
  gStyle->SetLabelOffset(0.01,"xyz");
  gStyle->SetTitleFont(font,"xyz");
  gStyle->SetTitleOffset(1.1,"xy");
  gStyle->SetTitleSize(0.04,"xyz");
  gStyle->SetMarkerSize(1.1);
  gStyle->SetLineWidth(2);
  gStyle->SetLegendFont(42);
  gStyle->SetLegendBorderSize(0);
  gStyle->SetLegendFillColor(10);
  can->SetTickx();
  can->SetTicky();
  gStyle->SetEndErrorSize(0);
  can->SetFillColor(0);
  can->SetBorderMode(0);
  can->SetBorderSize(0);
  can->SetLeftMargin(0.18);
  can->SetRightMargin(0.1);
  can->SetBottomMargin(0.1518219);
  // can->SetTopMargin(0.);
  can->SetFrameBorderMode(0);
}
