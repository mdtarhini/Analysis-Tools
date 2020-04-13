/*
This macro compare the pt and rapidity dependence of the corrected (or non-corrected) yields. A fit to the different yields was also added. 
*/
//--------------------------------------------------------------------------------------------//
#include "Common.C"
using namespace std;
Int_t multRanges[][2] = {{0, 100}, {1, 8}, {9, 14}, {15, 20}, {21, 25}, {26, 33}, {34, 41}, {42, 50}, {51, 60}, {61, 100}};
int numberOfMultRanges = sizeof(multRanges) / sizeof(multRanges[0]);

TFile *inputRawYieldFileVsRap = new TFile("RapRawYields.root");
TFile *inputRawYieldFileVsPt = new TFile("PtRawYields.root");

//--------------------------------------------------------------------------------------------//
void ComparePtYields(Bool_t isCorrected = kTRUE, int iteration = 0)
{
  TCanvas *canPtYields = new TCanvas("canPtYields", "", 1200, 800);
  SetCanvasStyle(canPtYields);
  canPtYields->SetLogy();

  TH1F *histoEmptyPt = new TH1F("histoEmptyPt", "", 10, 0, 20);
  SetHistoStyle(histoEmptyPt, kWhite, kFullCircle, 0.8, "#it{p}_{T}", isCorrected ? "dN^{corr}_{J/#psi}/d#it{p}_{T}" : "dN_{J/#psi}/d#it{p}_{T}", 1, kFALSE);
  histoEmptyPt->GetXaxis()->SetLabelSize(0.045);
  histoEmptyPt->GetXaxis()->SetTitleSize(0.045);
  histoEmptyPt->Draw();

  TLegend *legPt = new TLegend(0.68, 0.6, 0.95, 0.85);
  legPt->SetFillStyle(0);
  legPt->SetLineColorAlpha(0, 0);
  legPt->SetTextColor(kBlack);
  legPt->SetMargin(0.1);

  Double_t minPtYield = 10e9;
  Double_t maxPtYield = 0;

  for (int iMultBin = 0; iMultBin < numberOfMultRanges; iMultBin++)
  {
    canPtYields->cd();

    //Get the corresponding Acc*Effi file if the option "iscorrected"
    TFile *inputAccEffFile = new TFile(Form("Mult-%dto%d/AccEffi/iter-%d/AccEffiValues.root", multRanges[iMultBin][0], multRanges[iMultBin][1], iteration));
    TH1F *histoAccEffiVsPt = ((TH1F *)inputAccEffFile->Get("histoAccEffiVsPt"));

    TH1F *histoJpsiNumbersVsPt = ((TH1F *)inputRawYieldFileVsPt->Get(Form("histoPtYield_mult%dto%d", multRanges[iMultBin][0], multRanges[iMultBin][1])));
    if (isCorrected)
    {
      histoJpsiNumbersVsPt->Divide(histoAccEffiVsPt);
    }
    if (histoJpsiNumbersVsPt->GetMinimum() < minPtYield)
      minPtYield = histoJpsiNumbersVsPt->GetMinimum();
    if (histoJpsiNumbersVsPt->GetMaximum() > maxPtYield)
      maxPtYield = histoJpsiNumbersVsPt->GetMaximum();
    SetHistoStyle(histoJpsiNumbersVsPt, GetColorIndex(arrayColors[iMultBin]), kFullCircle, 0.8, "#it{p}_{T}", isCorrected ? "dN^{corr}_{J/#psi}/d#it{p}_{T}" : "dN_{J/#psi}/d#it{p}_{T}", 1, kFALSE);
    histoJpsiNumbersVsPt->Draw("same");
    TF1 *fPtDistribution = new TF1("fPtDistribution", "[0]*x/((1+(x/[1])**[2])**[3] )", 0, 20);
    fPtDistribution->SetParameter(0, histoJpsiNumbersVsPt->GetMaximum());
    fPtDistribution->SetParameter(1, 4);
    fPtDistribution->SetParameter(2, 2);
    fPtDistribution->SetParameter(3, 3);
    fPtDistribution->SetLineColorAlpha(GetColorIndex(arrayColors[iMultBin]), 0.75);
    fPtDistribution->SetLineWidth(1);
    TFitResultPtr r = histoJpsiNumbersVsPt->Fit(fPtDistribution, "REIS", "", 0, 20);
    legPt->AddEntry(histoJpsiNumbersVsPt, Form("%d #leq N_{trk}^{corr} #leq %d", multRanges[iMultBin][0], multRanges[iMultBin][1]), "p");
  }
  histoEmptyPt->GetYaxis()->SetRangeUser(minPtYield * 0.9, maxPtYield * 2);
  canPtYields->cd();
  legPt->Draw();
  canPtYields->SaveAs(Form("FinalPlots/PtYieldsVsMult_iter%d.pdf", iteration));
}
//--------------------------------------------------------------------------------------------//



//--------------------------------------------------------------------------------------------//
void CompareRapYields(Bool_t isCorrected = kTRUE, int iteration = 0)
{
  TCanvas *canRapYields = new TCanvas("canRapYields", "", 1200, 800);
  SetCanvasStyle(canRapYields);
  canRapYields->SetRightMargin(0.15);
  canRapYields->SetLogy();

  TH1F *histoEmptyRap = new TH1F("histoEmptyRap", "", 10, -4, -2.5);
  SetHistoStyle(histoEmptyRap, kWhite, kFullCircle, 0.8, "#it{y}", isCorrected ? "dN^{corr}_{J/#psi}/d#it{y}" : "dN_{J/#psi}/d#it{y}", 1, kFALSE);
  histoEmptyRap->GetXaxis()->SetLabelSize(0.045);
  histoEmptyRap->GetXaxis()->SetTitleSize(0.045);
  histoEmptyRap->Draw();

  TLegend *legRap = new TLegend(0.86, 0.33, 0.99, 0.69);
  legRap->SetFillStyle(0);
  legRap->SetLineColorAlpha(0, 0);
  legRap->SetTextColor(kBlack);
  legRap->SetMargin(0.1);

  Double_t minRapYield = 10e9;
  Double_t maxRapYield = 0;

  for (int iMultBin = 0; iMultBin < numberOfMultRanges; iMultBin++)
  {
    canRapYields->cd();

    //Get the corresponding Acc*Effi file if the option "iscorrected"
    TFile *inputAccEffFile = new TFile(Form("Mult-%dto%d/AccEffi/iter-%d/AccEffiValues.root", multRanges[iMultBin][0], multRanges[iMultBin][1], iteration));
    TH1F *histoAccEffiVsRap = ((TH1F *)inputAccEffFile->Get("histoAccEffiVsRap"));

    TH1F *histoJpsiNumbersVsRap = ((TH1F *)inputRawYieldFileVsRap->Get(Form("histoRapYield_mult%dto%d", multRanges[iMultBin][0], multRanges[iMultBin][1])));
    if (isCorrected)
    {
      histoJpsiNumbersVsRap->Divide(histoAccEffiVsRap);
    }
    if (histoJpsiNumbersVsRap->GetMinimum() < minRapYield)
      minRapYield = histoJpsiNumbersVsRap->GetMinimum();
    if (histoJpsiNumbersVsRap->GetMaximum() > maxRapYield)
      maxRapYield = histoJpsiNumbersVsRap->GetMaximum();
    SetHistoStyle(histoJpsiNumbersVsRap, GetColorIndex(arrayColors[iMultBin]), kFullCircle, 0.8, "#it{y}", isCorrected ? "dN^{corr}_{J/#psi}/d#it{y}" : "dN_{J/#psi}/d#it{y}", 1, kFALSE);
    histoJpsiNumbersVsRap->Draw("same");
    TF1 *fRapDistribution = new TF1("fRapDistribution","[0]*exp(-0.5*(x/[1])**2)",-4, -2.5);
    fRapDistribution->SetParameter(0, histoJpsiNumbersVsRap->GetMaximum());
    fRapDistribution->SetParameter(1, 3);
    fRapDistribution->SetLineColorAlpha(GetColorIndex(arrayColors[iMultBin]), 0.75);
    fRapDistribution->SetLineWidth(1);
    histoJpsiNumbersVsRap->Fit(fRapDistribution, "REIS", "", -4,-2.5);
    legRap->AddEntry(histoJpsiNumbersVsRap, Form("%d #leq N_{trk}^{corr} #leq %d", multRanges[iMultBin][0], multRanges[iMultBin][1]), "p");
  }
  histoEmptyRap->GetYaxis()->SetRangeUser(minRapYield * 0.9, maxRapYield * 2);
  canRapYields->cd();
  legRap->Draw();
  canRapYields->SaveAs(Form("FinalPlots/RapYieldsVsMult_iter%d.pdf", iteration));
}
//--------------------------------------------------------------------------------------------//