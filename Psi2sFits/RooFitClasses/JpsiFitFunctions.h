
#ifndef JPSIFITFUNCTIONS_H
#define JPSIFITFUNCTIONS_H

#include "RooGlobalFunc.h"
#include "TMath.h"
#include "TString.h"
#include "TROOT.h"
#include "TFrame.h"
#include "TObjArray.h"
#include "RooRealVar.h"
#include "RooDataHist.h"
#include "RooChi2Var.h"
#include "RooFitResult.h"
#include "RooAddPdf.h"
#include "RooExtendPdf.h"
#include "RooPlot.h"
#include "RooProduct.h"
#include "RooArgList.h"
// #include "DoubleCrystalBallShifted.h"
#include "DoubleCrystalBall.h"
#include "NASixty.h"
// #include "NASixtyShifted.h"
#include "VariableWidthGaus.h"
#include "VariableWidthGaus2.h"
#include "Pol2OverPol3.h"
#include "Pol1OverPol2.h"
#include "DoubleExpo.h"
#include "ExpPol2.h"
// #include "DoubleCrystalBallShifted.cxx"
#include "DoubleCrystalBall.cxx"
#include "NASixty.cxx"
// #include "NASixtyShifted.cxx"
#include "VariableWidthGaus.cxx"
#include "VariableWidthGaus2.cxx"
#include "Pol2OverPol3.cxx"
#include "Pol1OverPol2.cxx"
#include "DoubleExpo.cxx"
#include "ExpPol2.cxx"
#include "RooExponential.h"

class JpsiFitFunctions{

private:
  std::vector<double> fVectorTails;
  RooRealVar *fSigMean;
  RooRealVar *fSigWidth;
  RooRealVar *cbAlpha;
  RooRealVar *cbAlphaPrime;
  RooRealVar *cbn;
  RooRealVar *cbnPrime;
  TObjArray *fListOfFunctions; // list for the tree
  Bool_t   fIsJpsi;
  Int_t   fFixCBTails;
  Double_t fMCWidth;
  Double_t fRatioWidth2S;
  Double_t fRatioWidth3S;


public:
    JpsiFitFunctions();
    inline virtual ~JpsiFitFunctions() {}
    void DefineFunctions(RooRealVar *variable);
    //Getters
    TObjArray *GetListOfFunctions() const {return fListOfFunctions;}
    RooRealVar *GetSignalMean() const {return fSigMean;}
    RooRealVar *GetSignalWidth() const {return fSigWidth;}
    RooRealVar *GetCBAlpha() const {return cbAlpha;}
    RooRealVar *GetCBAlphaPrime() const {return cbAlphaPrime;}
    RooRealVar *GetCBn() const {return cbn;}
    RooRealVar *GetCBnPrime() const {return cbnPrime;}
    //Setters
    void IsJpsi(Bool_t isJpsi) {fIsJpsi=isJpsi;}
    void SetTails(std::vector<double> vectorTails);
    void FixCBTails(Int_t fixCBTails) {fFixCBTails=fixCBTails;}
    void SetMCWidth(Double_t mcWidth) {fMCWidth=mcWidth;}
    void Set2SWidthRatio(Double_t ratioWidth2s) {fRatioWidth2S=ratioWidth2s;}
    void Set3SWidthRatio(Double_t ratioWidth3s) {fRatioWidth3S=ratioWidth3s;}


    float JpsiMean = 3.096;
    float JpsiMeanMin = 3;
    float JpsiMeanMax = 3.2;

    float JpsiWidth = 0.068;
    float JpsiWidthMin = 0.05;
    float JpsiWidthMax = 0.2;

    float UpsilonMean = 9.44;
    float UpsilonMeanMin = 9;
    float UpsilonMeanMax = 10;

    float UpsilonWidth = 0.130;
    float UpsilonWidthMin = 0.05;
    float UpsilonWidthMax = 0.5;

    float CBAlpha = 1.053;
    float CBAlphaMin = 0;
    float CBAlphaMax = 5;

    float CBn = 3.287;
    float CBnMin = 0;
    float CBnMax = 5;

    float CBAlphaPrime = 2.148;
    float CBAlphaPrimeMin = 0;
    float CBAlphaPrimeMax = 5;

    float CBnPrime = 2.718;
    float CBnPrimeMin = 0;
    float CBnPrimeMax = 5;

    float BKGDSlope = 1;

    float VWGMean = 1.78;
    float VWGMeanMin = 0;
    float VWGMeanMax = 10;

    float VWGAlpha = 0.60;
    float VWGAlphaMin = 0;
    float VWGAlphaMax = 3;

    float VWGBeta = 0.28;
    float VWGBetaMin = -1;
    float VWGBetaMax = 1;

    float M_jpsi = 3.096916;
    float M_psi2s = 3.686109;

    float M_upsilon1s = 9.46030;
    float M_upsilon2s = 10.02326;
    float M_upsilon3s = 10.3552;

    enum enumFunctions{kCB21S,kNA601S,kCB22S,kNA602S,kCB23S,kNA603S,kVWG,kVWG2,kPol1OverPol2,kPol2OverPol3,kExpo,kDoubleExpo,kExpoPol2};
    enum fixTailsEnum {kFixNone,kFixAll, kFixLeft, kFixRight, kFixAlphas, kFixns};

    ClassDef(JpsiFitFunctions, 1);
};

#endif
