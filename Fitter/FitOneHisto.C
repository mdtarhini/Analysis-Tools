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

std::vector<double> FitOneHisto(TH1 *histoInvMass,int fSig, int fBkgd, TString strHistoTitle,TString strRange, Double_t fitRangeMin, Double_t fitRangeMax, std::vector<double> tailsParams, Bool_t fixCBTails, Double_t jpsiMCWidth, int rebiningFator){

  //----------------------------------------------------------------------------------------------//
  //Initialise the roofit variables and import the histogram:
  TCanvas* canFit = new TCanvas("canFit","",1000,900) ;
  SetCanvasStyle(canFit);
  // canFit->SetLogy();
  //----------------------------------------------------------------------------------------------//


  //----------------------------------------------------------------------------------------------//
  //Initialise the roofit variables and import the histogram:
  RooRealVar *invMass = new RooRealVar("invMass","M_{#mu #mu}",fitRangeMin,fitRangeMax,"GeV/c^{2}") ;
  RooPlot* frame = invMass->frame(Title(Form("%s, %s, Range: %2.2f,%2.2f",arrayFunctionNames[fSig].Data(),arrayFunctionNames[fBkgd].Data(),fitRangeMin,fitRangeMax))) ;

  histoInvMass->Rebin(rebiningFator);
  RooDataHist invMassData("invMassData","invMassData",*invMass,Import(*histoInvMass)) ;
  invMassData.plotOn(frame,Name("invMassData"),MarkerSize(0.8)) ;
  //----------------------------------------------------------------------------------------------//


  //----------------------------------------------------------------------------------------------//
  //Tails parameters:
  if(tailsParams.size() == 0){
    cout<<"Tail parameters are not set. Default ones will be used."<<endl;
    for(Int_t iTail=0;iTail<totalNumberOfTails;iTail++){
      tailsParams.push_back(arrayDefaultTails[iTail]);
    }
  }
  //----------------------------------------------------------------------------------------------//


  //----------------------------------------------------------------------------------------------//
  //Guess of the signal/back ratio
  int minMassBin = histoInvMass->FindBin(fitRangeMin);
  int maxMassBin = histoInvMass->FindBin(fitRangeMax);
  int minResonanceMassBin = histoInvMass->FindBin(fitRangeMin < 5 ? 2.9 : 9);
  int maxResonanceMassBin = histoInvMass->FindBin(fitRangeMin < 5 ? 3.2 : 10.5);

  Double_t approxNumberOfBkgd = histoInvMass->Integral(minMassBin,maxMassBin)-histoInvMass->Integral(minResonanceMassBin,maxResonanceMassBin);
  //----------------------------------------------------------------------------------------------//


  //----------------------------------------------------------------------------------------------//
  //Is it a loglikelihood fit ?
  if(!dontDoALogFit){
    int numberOfZeroBins =0;
    for(int iBin =0;iBin<(maxMassBin-minMassBin);iBin++){
      if(histoInvMass->GetBinContent(iBin+1+minMassBin) == 0)
      numberOfZeroBins++;
    }
    if(numberOfZeroBins != 0){
      doALogFit = kTRUE;
    }
  }
  //----------------------------------------------------------------------------------------------//


  //----------------------------------------------------------------------------------------------//
  //Start building the total fit function:
  JpsiFitFunctions *fitCollection  = new JpsiFitFunctions();
  fitCollection->IsJpsi(fitRangeMin < 5 ? kTRUE : kFALSE);
  fitCollection->SetTails(tailsParams);
  fitCollection->FixCBTails(fixCBTails);
  fitCollection->SetMCWidth(jpsiMCWidth);
  fitCollection->Set2SWidthRatio(1.05);
  fitCollection->DefineFunctions(invMass);


  TObjArray *arrayFitFunctions = fitCollection->GetListOfFunctions();
  RooAbsPdf *jpsiSigFuncion = ((RooAbsPdf*) arrayFitFunctions->UncheckedAt(fSig));
  RooAbsPdf *psipSigFuncion = ((RooAbsPdf*) arrayFitFunctions->UncheckedAt(fSig+2));
  RooAbsPdf *bkgdFuncion = ((RooAbsPdf*) arrayFitFunctions->UncheckedAt(fBkgd));


  RooRealVar NBkgd("NBkgd","NBkgd",approxNumberOfBkgd*0.5,0,approxNumberOfBkgd*10);
  RooRealVar NJpsi("NJpsi","NJpsi",histoInvMass->Integral(minMassBin,maxMassBin)-approxNumberOfBkgd,0,10*(histoInvMass->Integral(minMassBin,maxMassBin)-approxNumberOfBkgd));
  RooRealVar NPsi2s("Npsip","Npsip",(histoInvMass->Integral(minMassBin,maxMassBin)-approxNumberOfBkgd),0,2*(histoInvMass->Integral(minMassBin,maxMassBin)-approxNumberOfBkgd));
  RooAddPdf  *dimuonModel = new RooAddPdf("dimuonModel","dimuonModel",RooArgList(*bkgdFuncion,*jpsiSigFuncion,*psipSigFuncion),RooArgList(NBkgd,NJpsi,NPsi2s)) ;
  // RooAddPdf  *dimuonModel = new RooAddPdf("dimuonModel","dimuonModel",RooArgList(*bkgdFuncion,*jpsiSigFuncion),RooArgList(NBkgd,NJpsi)) ;
  //----------------------------------------------------------------------------------------------//


  //----------------------------------------------------------------------------------------------//
  //Do an iterative fit. Iterative means to try different initial conditions depending on the chisquare of the fit.
  Double_t bkgdFraction0 = 1;
  Double_t chi2OverNdf;
  RooFitResult *fitRes;

  for(Double_t bkgdFraction = bkgdFraction0;bkgdFraction<10;bkgdFraction = bkgdFraction+0.1){
    NBkgd.setVal(approxNumberOfBkgd*bkgdFraction);
    if(doALogFit) fitRes = dimuonModel->fitTo(invMassData,Extended(kTRUE), Save(kTRUE)) ;
    else fitRes = dimuonModel->chi2FitTo(invMassData,Extended(kTRUE), Save(kTRUE)) ;
    RooChi2Var chi2 ("chi2", "chi2", *dimuonModel, invMassData);
    Double_t numberOfFloatPars = fitRes->floatParsFinal().getSize();
    Double_t ndf = invMassData.numEntries() - numberOfFloatPars - 1;
    chi2OverNdf = chi2.getVal()/ndf;
    if ( (chi2OverNdf < 3) ){
      break;
    }
  }

  //Calculate the S/B of the result fit:
  RooRealVar *sigMean = fitCollection->GetSignalMean();
  RooRealVar *sigWidth = fitCollection->GetSignalWidth();

  Float_t SignalOverBckgd;
  invMass->setRange("signalRange",sigMean->getVal()-3*sigWidth->getVal(),sigMean->getVal()+3*sigWidth->getVal()) ;
  RooAbsReal* SignalIntegral = jpsiSigFuncion->createIntegral(*invMass,NormSet(*invMass),Range("signalRange")) ;
  RooAbsReal* BckgdVWGIntegral = bkgdFuncion->createIntegral(*invMass,NormSet(*invMass),Range("signalRange")) ;
  SignalOverBckgd = (SignalIntegral->getVal()*NJpsi.getVal())/(BckgdVWGIntegral->getVal()*NBkgd.getVal());
  //----------------------------------------------------------------------------------------------//


  //----------------------------------------------------------------------------------------------//
  //Plotting and stuff:
  dimuonModel->plotOn(frame,Components(*jpsiSigFuncion),LineColor(kRed),LineStyle(7),LineWidth(2)) ;
  dimuonModel->plotOn(frame,Components(*psipSigFuncion),LineColor(kGreen+2),LineStyle(7),LineWidth(2)) ;
  dimuonModel->plotOn(frame,Components(*bkgdFuncion),LineColor(kGray),LineStyle(7)) ;
  dimuonModel->plotOn(frame,Name("dimuonModel"),LineColor(kBlue)) ;

  frame->GetYaxis()->SetTitleOffset(1.7);
  frame->Draw("") ;

  TLegend* legFit = new TLegend(0.502, 0.534, 0.872, 0.883);
  legFit->SetFillStyle(0);
  legFit->SetLineColorAlpha(0,0);
  legFit->SetTextColor(kBlack);
  legFit->SetMargin(0.1);
  legFit->AddEntry((TObject*)0,Form("M_{J/#psi} = %4.4f #pm %4.4f Gev/c^{2}",(sigMean->getVal()),(sigMean->getError())) , "");
  legFit->AddEntry((TObject*)0,Form("#sigma_{J/#psi} = %4.4f #pm %4.4f Gev/c^{2}",(sigWidth->getVal()),(sigWidth->getError())) , "");
  legFit->AddEntry((TObject*)0,Form("#chi^{2}/ndf = %2.2f", chi2OverNdf), "");
  legFit->AddEntry((TObject*)0,Form("S/B_{3#sigma} = %2.2f", SignalOverBckgd), "");
  legFit->AddEntry((TObject*)0,Form("N_{J/#psi} = %d #pm %d",(int)(NJpsi.getVal()),(int)(NJpsi.getError())) , "");
  legFit->AddEntry((TObject*)0,Form("N_{#psi(2s)} = %d #pm %d",(int)(NPsi2s.getVal()),(int)(NPsi2s.getError())) , "");
  legFit->Draw();
  canFit->SaveAs(Form("psi2s2018Plots/%s_%s_Range_%2.2f_%2.2f.pdf",arrayFunctionNames[fSig].Data(),arrayFunctionNames[fBkgd].Data(),fitRangeMin,fitRangeMax));
  //----------------------------------------------------------------------------------------------//


  //----------------------------------------------------------------------------------------------//
  //Return the results of the fit in a vector:
  std::vector<double> fitResults;
  fitResults.push_back(NJpsi.getVal());
  fitResults.push_back(NJpsi.getError());
  fitResults.push_back(sigMean->getVal());
  fitResults.push_back(sigMean->getError());
  fitResults.push_back(sigWidth->getVal());
  fitResults.push_back(sigWidth->getError());
  fitResults.push_back(chi2OverNdf);
  return fitResults;
  //----------------------------------------------------------------------------------------------//

}
