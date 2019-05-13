#include "FitOneHisto.C"

void FitTest()
{
  TFile *inputFile = new TFile("ResultsDimuon.root");
  TH2F *histo2DDimuonInvMassVsPt = ((TH2F*) inputFile->Get("histo2DDimuonInvMassVsPt"));
  TH1D *histoInvMass = histo2DDimuonInvMassVsPt->ProjectionY("InvMassWeighted",0,400);
  //

  // TFile *inputFile = new TFile("HistosfromTree_LHC18qr.root");
  // TH1D *histoInvMass = ((TH1D*) inputFile->Get("hMassOS_CMUL7_2m"));
  // histoInvMass->Rebin(2);

  std::vector<double> tailsParams;
  //FitOneHisto(TH1 *histoInvMass,int fSig, int fBkgd, TString strHistoTitle,TString strRange, Double_t fitRangeMin, Double_t fitRangeMax, std::vector<double> tailsParams, Bool_t fixCBTails, Double_t jpsiMCWidth, int rebiningFator)
  FitOneHisto(histoInvMass,kCB21S, kVWG, "test","test", 2.4,4.7,tailsParams,kTRUE,-1, 4);

}
