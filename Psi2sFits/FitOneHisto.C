/*
This is a simple macro to fit one dimuon histogram. Keep this macro as simple as possible, do not include histogram and range selection.
The fit method is based on RooFit. The signal and background functions are sent as argument for the main function and will be taken from the JpsiFitFunctions class.
*/

#include "RooFitClasses/JpsiFitFunctions.h"
#include "RooFitClasses/JpsiFitFunctions.cxx"
#include "Common.C"
using namespace std;
using namespace RooFit;
///////////////////////////////////////////

Bool_t dontDoALogFit = kTRUE;
Bool_t doALogFit = kFALSE;

void FitOneHisto(TString canvasName, TH1 *histoInvMass, int fSig, Double_t psi2sWidth, int fTails, int fBkgd, Double_t fitRangeMin, Double_t fitRangeMax, Double_t centMin, Double_t centMax, Double_t ptMin, Double_t ptMax, Double_t rapMin, Double_t rapMax, Bool_t setLogY, std::vector<double> &vectorResults, std::vector<double> &vectorResultsErr)
{

  //----------------------------------------------------------------------------------------------//
  //Initialise the roofit variables and import the histogram:
  TCanvas *canFit = new TCanvas(canvasName, "", 1000, 900);
  SetCanvasStyle(canFit);
  if (setLogY)
    canFit->SetLogy();
  //----------------------------------------------------------------------------------------------//

  //----------------------------------------------------------------------------------------------//
  //Initialise the roofit variables and import the histogram:
  RooRealVar *invMass = new RooRealVar("invMass", "#it{M}_{#mu#mu}", fitRangeMin, fitRangeMax, "GeV/#it{c}^{2}");
  RooPlot *frame = invMass->frame();
  RooDataHist invMassData("invMassData", "invMassData", *invMass, Import(*histoInvMass));
  invMassData.plotOn(frame, Name("invMassData"), MarkerSize(0.8), MarkerColor(TColor::GetColor(strHexMap["darkBlue"])));
  //----------------------------------------------------------------------------------------------//

  //----------------------------------------------------------------------------------------------//
  //Tails parameters:
  std::vector<double> tailsParams = GetTails(fSig, fTails, ptMin, ptMax, rapMin, rapMax);
  //----------------------------------------------------------------------------------------------//

  //----------------------------------------------------------------------------------------------//
  //Guess of the signal/back ratio
  int minMassBin = histoInvMass->FindBin(fitRangeMin);
  int maxMassBin = histoInvMass->FindBin(fitRangeMax);
  int minResonanceMassBin = histoInvMass->FindBin(fitRangeMin < 5 ? 2.9 : 9);
  int maxResonanceMassBin = histoInvMass->FindBin(fitRangeMin < 5 ? 3.2 : 10.5);

  Double_t approxNumberOfBkgd = histoInvMass->Integral(minMassBin, maxMassBin) - histoInvMass->Integral(minResonanceMassBin, maxResonanceMassBin);
  //----------------------------------------------------------------------------------------------//

  //----------------------------------------------------------------------------------------------//
  //Is it a loglikelihood fit ?
  if (!dontDoALogFit)
  {
    int numberOfZeroBins = 0;
    for (int iBin = 0; iBin < (maxMassBin - minMassBin); iBin++)
    {
      if (histoInvMass->GetBinContent(iBin + 1 + minMassBin) == 0)
        numberOfZeroBins++;
    }
    if (numberOfZeroBins != 0)
    {
      doALogFit = kTRUE;
    }
  }
  //----------------------------------------------------------------------------------------------//

  //----------------------------------------------------------------------------------------------//
  //Start building the total fit function:
  JpsiFitFunctions *fitCollection = new JpsiFitFunctions();
  fitCollection->IsJpsi(fitRangeMin < 5 ? kTRUE : kFALSE);
  fitCollection->SetTails(tailsParams);
  fitCollection->FixCBTails(kFixAll);
  fitCollection->SetMCWidth(-1);
  fitCollection->Set2SWidthRatio(psi2sWidth);
  fitCollection->DefineFunctions(invMass);

  TObjArray *arrayFitFunctions = fitCollection->GetListOfFunctions();
  RooAbsPdf *jpsiSigFuncion = ((RooAbsPdf *)arrayFitFunctions->UncheckedAt(fSig));
  RooAbsPdf *psipSigFuncion = ((RooAbsPdf *)arrayFitFunctions->UncheckedAt(fSig + 2));
  RooAbsPdf *bkgdFuncion = ((RooAbsPdf *)arrayFitFunctions->UncheckedAt(fBkgd));

  RooRealVar NBkgd("NBkgd", "NBkgd", approxNumberOfBkgd * 0.5, 0, approxNumberOfBkgd * 10);
  RooRealVar NJpsi("NJpsi", "NJpsi", histoInvMass->Integral(minMassBin, maxMassBin) - approxNumberOfBkgd, 0, 10 * (histoInvMass->Integral(minMassBin, maxMassBin) - approxNumberOfBkgd));
  RooRealVar NPsi2s("Npsip", "Npsip", (histoInvMass->Integral(minMassBin, maxMassBin) - approxNumberOfBkgd), 0, 2 * (histoInvMass->Integral(minMassBin, maxMassBin) - approxNumberOfBkgd));
  RooAddPdf *dimuonModel = new RooAddPdf("dimuonModel", "dimuonModel", RooArgList(*bkgdFuncion, *jpsiSigFuncion, *psipSigFuncion), RooArgList(NBkgd, NJpsi, NPsi2s));
  //----------------------------------------------------------------------------------------------//

  //----------------------------------------------------------------------------------------------//
  //Do an iterative fit. Iterative means to try different initial conditions depending on the status of the fit.
  Double_t bkgdFraction0 = 1;
  Double_t chi2OverNdf;
  RooFitResult *fitRes;

  for (Double_t bkgdFraction = bkgdFraction0; bkgdFraction < 10; bkgdFraction = bkgdFraction + 0.1)
  {
    NBkgd.setVal(approxNumberOfBkgd * bkgdFraction);
    if (doALogFit)
      fitRes = dimuonModel->fitTo(invMassData, FitOptions(""), Extended(kTRUE), Save(kTRUE));
    else
      fitRes = dimuonModel->chi2FitTo(invMassData, FitOptions(""), Extended(kTRUE), Save(kTRUE));
    if (!fitRes)
      continue;
    RooChi2Var chi2("chi2", "chi2", *dimuonModel, invMassData);
    Double_t numberOfFloatPars = fitRes->floatParsFinal().getSize();
    Double_t ndf = invMassData.numEntries() - numberOfFloatPars - 1;
    chi2OverNdf = chi2.getVal() / ndf;
    if ((fitRes->status() == 0) && (fitRes->covQual() == 3))
    {
      break;
    }
  }
  //----------------------------------------------------------------------------------------------//

  //----------------------------------------------------------------------------------------------//
  //J/psi and pis(2s) S/B and significance:
  RooRealVar *sigMean = fitCollection->GetSignalMean();
  RooRealVar *sigWidth = fitCollection->GetSignalWidth();

  //J/psi:
  invMass->setRange("JpsiSignalRange", (sigMean->getVal()) - 3 * sigWidth->getVal(), (sigMean->getVal()) + 3 * sigWidth->getVal());
  RooAbsReal *jpsiSignalIntegral = jpsiSigFuncion->createIntegral(*invMass, NormSet(*invMass), Range("JpsiSignalRange"));
  RooAbsReal *jpsiBckgdIntegral = bkgdFuncion->createIntegral(*invMass, NormSet(*invMass), Range("JpsiSignalRange"));

  //S/B
  Float_t jpsiSignalOverBackground = (jpsiSignalIntegral->getVal() * NJpsi.getVal()) / (jpsiBckgdIntegral->getVal() * NBkgd.getVal());
  Float_t jpsiSignalOverBackground_err = (NJpsi.getError() / NJpsi.getVal()) * jpsiSignalOverBackground;

  //Significance
  Float_t jpsiSignificance = ((jpsiSignalIntegral->getVal() * NJpsi.getVal())) / TMath::Sqrt((jpsiSignalIntegral->getVal() * NJpsi.getVal()) + (jpsiBckgdIntegral->getVal() * NBkgd.getVal()));
  Float_t jpsiSignificance_err = (NJpsi.getError() / NJpsi.getVal()) * jpsiSignificance;

  //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//

  //Psi(2s):
  invMass->setRange("Psi2sSignalRange", (sigMean->getVal() + 0.589) - 3 * psi2sWidth * sigWidth->getVal(), (sigMean->getVal() + 0.589) + 3 * psi2sWidth * sigWidth->getVal());
  RooAbsReal *Psi2sSignalIntegral = psipSigFuncion->createIntegral(*invMass, NormSet(*invMass), Range("Psi2sSignalRange"));
  RooAbsReal *Psi2sBckgdVWGIntegral = bkgdFuncion->createIntegral(*invMass, NormSet(*invMass), Range("Psi2sSignalRange"));

  //S/B
  Float_t Psi2sSignalOverBackground = (Psi2sSignalIntegral->getVal() * NPsi2s.getVal()) / (Psi2sBckgdVWGIntegral->getVal() * NBkgd.getVal());
  Float_t Psi2sSignalOverBackground_err = (NPsi2s.getError() / NPsi2s.getVal()) * Psi2sSignalOverBackground;

  //Significance
  Float_t Psi2sSignificance = ((Psi2sSignalIntegral->getVal() * NPsi2s.getVal())) / TMath::Sqrt((Psi2sSignalIntegral->getVal() * NPsi2s.getVal()) + (Psi2sBckgdVWGIntegral->getVal() * NBkgd.getVal()));
  Float_t Psi2sSignificance_err = (NPsi2s.getError() / NPsi2s.getVal()) * Psi2sSignificance;
  //----------------------------------------------------------------------------------------------//

  //----------------------------------------------------------------------------------------------//
  //Plotting and stuff:
  dimuonModel->plotOn(frame, Components(*jpsiSigFuncion), LineColor(TColor::GetColor(strHexMap["red"])), LineStyle(7), LineWidth(2));
  dimuonModel->plotOn(frame, Components(*psipSigFuncion), LineColor(TColor::GetColor(strHexMap["green"])), LineStyle(7), LineWidth(2));
  dimuonModel->plotOn(frame, Components(*bkgdFuncion), LineColor(TColor::GetColor(strHexMap["grey"])), LineStyle(7));
  dimuonModel->plotOn(frame, Name("dimuonModel"), LineColor(TColor::GetColor(strHexMap["blue"])), LineWidth(1));
  frame->GetYaxis()->SetTitleOffset(1.7);
  frame->Draw("");
  //----------------------------------------------------------------------------------------------//

  //----------------------------------------------------------------------------------------------//
  int textColor = kBlack;
  if ((fitRes->status() != 0) || (fitRes->covQual() != 3) || chi2OverNdf > 3)
  {
    textColor = TColor::GetColor(strHexMap["redError"]);
  }
  //Legends and texts:
  //Range:
  AddAText(Form("Pb-Pb, %g-%g %% \n %g < #it{y} < %g, %g < #it{p}_{T} < %g", centMin, centMax, rapMin, rapMax, ptMin, ptMax), 0.188, 0.174, 0.368, 0.274, textColor);
  //Fit status:
  AddAText(Form("Status: %d, Cov-Q: %d \n #chi^{2}/ndf = %2.2f", fitRes->status(), fitRes->covQual(), chi2OverNdf), 0.188, 0.807, 0.5, 0.879, textColor);
  //J/psi info:
  AddAText(Form("#it{M}_{J/#psi} = %2.2f #pm %3.3f Gev/c^{2} \n #sigma_{J/#psi} = %2.2f #pm %2.2f MeV/c^{2} \n N_{J/#psi} = %d #pm %d \n S/B_{3#sigma} = %2.2f", (sigMean->getVal()), (sigMean->getError()), 1000 * (sigWidth->getVal()), 1000 * (sigWidth->getError()), (int)(NJpsi.getVal()), (int)(NJpsi.getError()), jpsiSignalOverBackground), 0.60, 0.87, 0.83, 0.72, textColor);

  //psi(2s) info:
  AddAText(Form("#sigma_{#psi(2s)} = %2.2f#times#sigma_{J/#psi} \n N_{#psi(2s)} = %d #pm %d \n S/B_{3#sigma} = %3.3f \n S/#sqrt{S+B}_{3#sigma} = %2.2f", psi2sWidth, (int)(NPsi2s.getVal()), (int)(NPsi2s.getError()), Psi2sSignalOverBackground, Psi2sSignificance), 0.68, 0.26, 0.91, 0.43, textColor);

  TString filename = canvasName;
  filename.ReplaceAll(" ", "_");
  canFit->SaveAs(Form("%s.pdf", filename.Data()));
  //----------------------------------------------------------------------------------------------//

  //----------------------------------------------------------------------------------------------//
  //Return the results of the fit in a vector:
  vectorResults.push_back(NJpsi.getVal());
  vectorResults.push_back(NPsi2s.getVal());
  vectorResults.push_back(sigMean->getVal());
  vectorResults.push_back(sigWidth->getVal());
  vectorResults.push_back(chi2OverNdf);
  vectorResults.push_back(fitRes->status());
  vectorResults.push_back(fitRes->covQual());

  vectorResultsErr.push_back(NJpsi.getError());
  vectorResultsErr.push_back(NPsi2s.getError());
  vectorResultsErr.push_back(sigMean->getError());
  vectorResultsErr.push_back(sigWidth->getError());
  vectorResultsErr.push_back(0);
  vectorResultsErr.push_back(0);
  vectorResultsErr.push_back(0);
  // delete canFit;

  //----------------------------------------------------------------------------------------------//
}
