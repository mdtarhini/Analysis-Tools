#include "JpsiFitFunctions.h"

using namespace std;
using namespace RooFit;

ClassImp(JpsiFitFunctions)

    JpsiFitFunctions::JpsiFitFunctions() {
  cout << "Default constructor is not yet defined" << endl;
}

void JpsiFitFunctions::DefineFunctions(RooRealVar *variable) {
  //----------------------------------------------------------------------------------------------//
  // Default class parameters:
  // fIsJpsi = kTRUE;
  // fFixCBTails = kTRUE;
  // fMCWidth = -1;
  // fRatioWidth2S = 1.05;
  // fRatioWidth3S = 1.08;
  //----------------------------------------------------------------------------------------------//

  //----------------------------------------------------------------------------------------------//
  // Tails parameters:
  if (fVectorTails.empty()) {
    cout << "The provided tail vector is empty" << endl;
    return;
  }
  for (Int_t iTail = 0; iTail < fVectorTails.size(); iTail++) {
    cout << Form("Tail parameter %d: %4.4f", iTail, fVectorTails[iTail]);
  }
  //----------------------------------------------------------------------------------------------//

  //----------------------------------------------------------------------------------------------//
  // Width manipulation: For now, the two options are fixing to MC or letting it
  // free. In both cases, the signal width and pole positions concerned the 1s
  // state, while ns are tied to the 1s
  fSigMean =
      new RooRealVar("sigMean", "sigMean", fIsJpsi ? JpsiMean : UpsilonMean,
                     fIsJpsi ? JpsiMeanMin : UpsilonMeanMin,
                     fIsJpsi ? JpsiMeanMax : UpsilonMeanMax);
  if (fMCWidth < 0) {
    fSigWidth = new RooRealVar("sigWidth", "sigWidth",
                               fIsJpsi ? JpsiWidth : UpsilonWidth,
                               fIsJpsi ? JpsiWidthMin : UpsilonWidthMin,
                               fIsJpsi ? JpsiWidthMax : UpsilonWidthMax);
  } else {
    fSigWidth = new RooRealVar("sigWidth", "sigWidth", fMCWidth, fMCWidth - 1,
                               fMCWidth + 1);
    fSigWidth->setConstant(kTRUE);
    return;
  }
  //----------------------------------------------------------------------------------------------//

  //----------------------------------------------------------------------------------------------//
  // Initialise the function container
  fListOfFunctions = new TObjArray(100);
  fListOfFunctions->SetOwner(kTRUE);
  fListOfFunctions->SetName("arrayFitFunctions");

  // In the case of a crystal ball as signal function, define roovariables,
  // which is not the case for NA60 due to limit on the passable arguments in
  // roofit
  cbAlpha = new RooRealVar("cbAlpha", "cbAlpha", fVectorTails[0],
                                       CBAlphaMin, CBAlphaMax);
  cbn = new RooRealVar("cbn", "cbn", fVectorTails[1], CBnMin, 100);
  cbAlphaPrime =
      new RooRealVar("cbAlphaPrime", "cbAlphaPrime", fVectorTails[2],
                     CBAlphaPrimeMin, CBAlphaPrimeMax);
  cbnPrime =
      new RooRealVar("cbnPrime", "cbnPrime", fVectorTails[3], CBnPrimeMin, 100);

  if (fFixCBTails == kFixAll) {
    cbAlpha->setConstant(kTRUE);
    cbn->setConstant(kTRUE);
    cbAlphaPrime->setConstant(kTRUE);
    cbnPrime->setConstant(kTRUE);
  } else if (fFixCBTails == kFixLeft) {
    cbAlpha->setConstant(kTRUE);
    cbn->setConstant(kTRUE);
  } else if (fFixCBTails == kFixRight) {
    cbAlphaPrime->setConstant(kTRUE);
    cbnPrime->setConstant(kTRUE);
  } else if (fFixCBTails == kFixAlphas) {
    cbAlphaPrime->setConstant(kTRUE);
    cbAlpha->setConstant(kTRUE);
  }
  else if (fFixCBTails == kFixns) {
    cbnPrime->setConstant(kTRUE);
    cbn->setConstant(kTRUE);
  }
  

  //----------------------------------------------------------------------------------------------//

  //----------------------------------------------------------------------------------------------//
  // definition of different signal functions and adding them to the list
  // For the signal function; tie the three resonnaces together by means of pole
  // position and sigmas
  //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
  // 1s CB2 signal
  DoubleCrystalBall *CB2Signal1S = new DoubleCrystalBall(
      "CB2Signal1S", "CB2Signal1S", *variable, *fSigMean, 0, *fSigWidth, 1,
      *cbAlpha, *cbn, *cbAlphaPrime, *cbnPrime);
  fListOfFunctions->AddAt(CB2Signal1S, kCB21S);
  //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
  // 2s CB2 signal (psi2s or upsilon(2s))
  DoubleCrystalBall *CB2Signal2S = new DoubleCrystalBall(
      "CB2Signal2S", "CB2Signal2S", *variable, *fSigMean,
      fIsJpsi ? M_psi2s - M_jpsi : M_upsilon2s - M_upsilon1s, *fSigWidth,
      fRatioWidth2S, *cbAlpha, *cbn, *cbAlphaPrime, *cbnPrime);
  fListOfFunctions->AddAt(CB2Signal2S, kCB22S);
  //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
  // 3s CB2 signal (upsilon(3s))
  DoubleCrystalBall *CB2Signal3S = new DoubleCrystalBall(
      "CB2Signal3S", "CB2Signal3S", *variable, *fSigMean,
      M_upsilon3s - M_upsilon1s, *fSigWidth, fRatioWidth3S, *cbAlpha, *cbn,
      *cbAlphaPrime, *cbnPrime);
  fListOfFunctions->AddAt(CB2Signal2S, kCB22S);
  //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
  NASixty *NA60Signal1S =
      new NASixty("NA60Signal1S", "NA60Signal1S", *variable, fVectorTails[0],
                  fVectorTails[1], fVectorTails[2], fVectorTails[3],
                  fVectorTails[4], fVectorTails[5], fVectorTails[6],
                  fVectorTails[7], *fSigMean, 0, *fSigWidth, 1);
  fListOfFunctions->AddAt(NA60Signal1S, kNA601S);
  //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
  NASixty *NA60Signal2S = new NASixty(
      "NA60Signal2S", "NA60Signal2S", *variable, fVectorTails[0],
      fVectorTails[1], fVectorTails[2], fVectorTails[3], fVectorTails[4],
      fVectorTails[5], fVectorTails[6], fVectorTails[7], *fSigMean,
      fIsJpsi ? M_psi2s - M_jpsi : M_upsilon2s - M_upsilon1s, *fSigWidth,
      fRatioWidth2S);
  fListOfFunctions->AddAt(NA60Signal2S, kNA602S);
  //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
  NASixty *NA60Signal3S = new NASixty(
      "NA60Signal3S", "NA60Signal3S", *variable, fVectorTails[0],
      fVectorTails[1], fVectorTails[2], fVectorTails[3], fVectorTails[4],
      fVectorTails[5], fVectorTails[6], fVectorTails[7], *fSigMean,
      M_upsilon3s - M_upsilon1s, *fSigWidth, fRatioWidth3S);
  fListOfFunctions->AddAt(NA60Signal3S, kNA603S);
  //----------------------------------------------------------------------------------------------//

  //----------------------------------------------------------------------------------------------//
  // definition of different bkgd functions and adding them to the list
  //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
  RooRealVar *vwgMean = new RooRealVar("vwgMean", "vwgMean", 1.78, 0, 10);
  RooRealVar *vwgAlpha = new RooRealVar("vwgAlpha", "vwgAlpha", 0.60, 0, 3);
  RooRealVar *vwgBeta = new RooRealVar("vwgBeta", "vwgBeta", 0.28, -1, 1);
  RooRealVar *vwgGamma = new RooRealVar("vwgGamma", "vwgGamma", 1, -10, 10);
  //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
  // Linearly Variable width gaussian
  VariableWidthGaus *bkgdVWGFunction = new VariableWidthGaus(
      "bkgdVWGFunction", "bkgdVWGFunction", *variable, *vwgMean, *vwgAlpha,
      *vwgBeta); // 1st bckgd function
  fListOfFunctions->AddAt(bkgdVWGFunction, kVWG);
  //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
  // quadratically Variable width gaussian
  VariableWidthGaus2 *bkgdVWG2Function =
      new VariableWidthGaus2("bkgdVWG2Function", "bkgdVWG2Function", *variable,
                             *vwgMean, *vwgAlpha, *vwgBeta, *vwgGamma);
  fListOfFunctions->AddAt(bkgdVWG2Function, kVWG2);
  //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//

  //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
  RooRealVar *A0 = new RooRealVar("A0", "A0", 26, -10, 100);
  RooRealVar *A1 = new RooRealVar("A1", "A1", -9, -30, 30);
  RooRealVar *A2 = new RooRealVar("A2", "A2", 0.9, -10, 10);
  RooRealVar *A3 = new RooRealVar("A3", "A3", 0.142, -10, 10);
  RooRealVar *B1 = new RooRealVar("B1", "B1", 9, -30, 30);
  RooRealVar *B2 = new RooRealVar("B2", "B2", -6, -10, 10);
  RooRealVar *B3 = new RooRealVar("B3", "B3", 1., -10, 10);
  Pol2OverPol3 *bkgdPol2OverPol3Function =
      new Pol2OverPol3("bkgdPol2OverPol3Function", "bkgdPol2OverPol3Function",
                       *variable, *A0, *A1, *A2, *B1, *B2, *B3);
  RooMsgService::instance().setGlobalKillBelow(RooFit::FATAL);
  fListOfFunctions->AddAt(bkgdPol2OverPol3Function, kPol2OverPol3);
  //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//

  //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
  RooRealVar *D0 = new RooRealVar("D0", "D0", 1, -10, 100);
  RooRealVar *D1 = new RooRealVar("D1", "D1", 1, -100, 100);
  RooRealVar *E1 = new RooRealVar("E1", "E1", 1, -1000, 1000);
  RooRealVar *E2 = new RooRealVar("E2", "E2", 1, -100, 100);
  Pol1OverPol2 *bkgdPol1OverPol2Function =
      new Pol1OverPol2("bkgdPol1OverPol2Function", "bkgdPol1OverPol2Function",
                       *variable, *D0, *D1, *E1, *E2);
  fListOfFunctions->AddAt(bkgdPol1OverPol2Function, kPol1OverPol2);
  //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//

  //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
  RooRealVar *expoSlope1 =
      new RooRealVar("expoSlope1", "expoSlope1", 1, -10, 10);
  RooRealVar *expoSlope2 =
      new RooRealVar("expoSlope2", "expoSlope2", 1, -10, 10);
  RooRealVar *expoAlpha =
      new RooRealVar("expoAlpha", "expoAlpha", 1, -100, 100);
  RooExponential *bkgdExpoFunction = new RooExponential(
      "bkgdExpoFunction", "bkgdExpoFunction", *variable, *expoSlope1);
  DoubleExpo *bkgdDoubleExpoFunction =
      new DoubleExpo("bkgdDoubleExpoFunction", "bkgdDoubleExpoFunction",
                     *variable, *expoSlope1, *expoAlpha, *expoSlope2);
  fListOfFunctions->AddAt(bkgdExpoFunction, kExpo);
  fListOfFunctions->AddAt(bkgdDoubleExpoFunction, kDoubleExpo);
  //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//

  //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
  RooRealVar *C0 = new RooRealVar("C0", "C0", 30, -100, 100);
  RooRealVar *C1 = new RooRealVar("C1", "C1", 0.9, -100, 100);
  RooRealVar *expoSlope3 =
      new RooRealVar("expoSlope3", "expoSlope3", -6, -30, 10);
  ExpPol2 *bkgdExpPol2Function =
      new ExpPol2("bkgdExpPol2Function", "bkgdExpPol2Function", *variable, *C0,
                  *C1, *expoSlope3);
  fListOfFunctions->AddAt(bkgdExpPol2Function, kExpoPol2);
  //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
}
void JpsiFitFunctions::SetTails(std::vector<double> vectorTails) {
  for (Int_t iTail = 0; iTail < (int)vectorTails.size(); iTail++) {
    fVectorTails.push_back(vectorTails[iTail]);
  }
}
