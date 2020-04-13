/*
This macro compare the pt and rapidity shapes obtained for various iteration and multiplicity bins. It has two modes for drawing:
*/
//--------------------------------------------------------------------------------------------//
#include "GetWeights.C"
using namespace std;
Int_t multRanges[][2] = {{0, 100}, {1, 8}, {9, 14}, {15, 20}, {21, 25}, {26, 33}, {34, 41}, {42, 50}, {51, 60}, {61, 100}};
int numberOfMultRanges = sizeof(multRanges) / sizeof(multRanges[0]);


//--------------------------------------------------------------------------------------------//
//In the first case, compare for each iteration the different input shapes of the different multipliity bins
void CompareMultShapesPerIteration()
{

  //Get the last iteration for the first multiplicity bin (for now assume it is the same for all the bins):
  TString strLatestIter = GetLatestIteration(multRanges[0][0], multRanges[0][1]);
  int latestIter = ((TString)strLatestIter(strLatestIter.Length() - 1, strLatestIter.Length() - 1)).Atoi();

  for (int iIteration = 1; iIteration <= latestIter; iIteration++)
  {

    TCanvas *canRapShapes = new TCanvas(Form("canRapShapes_iter%d", iIteration), "", 800, 800);
    SetCanvasStyle(canRapShapes);
    TH1F *histoEmptyRap = new TH1F(Form("histoEmptyRap_iter%d", iIteration), "", 10, -4, -2.5);
    SetHistoStyle(histoEmptyRap, kWhite, kFullCircle, 0.8, "#it{y}", "dN/d#it{y}", 1, kFALSE);
    histoEmptyRap->GetXaxis()->SetLabelSize(0.045);
    histoEmptyRap->GetXaxis()->SetTitleSize(0.045);
    histoEmptyRap->GetYaxis()->SetRangeUser(0.35, 1);
    histoEmptyRap->Draw();

    TLegend *legRap = new TLegend(0.639, 0.185, 0.889, 0.536);
    legRap->SetFillStyle(0);
    legRap->SetLineColorAlpha(0, 0);
    legRap->SetTextColor(kBlack);
    legRap->SetMargin(0.1);
    legRap->AddEntry("NULL", Form("Iteration %d", iIteration), "");

    TCanvas *canPtShapes = new TCanvas(Form("canPtShapes_iter%d", iIteration), "", 800, 800);
    SetCanvasStyle(canPtShapes);
    canPtShapes->SetLogy();
    TH1F *histoEmptyPt = new TH1F(Form("histoEmptyPt_iter%d", iIteration), "", 10, 0, 20);
    SetHistoStyle(histoEmptyPt, kWhite, kFullCircle, 0.8, "#it{p}_{T}", "dN/d#it{p}_{T}", 1, kFALSE);
    histoEmptyPt->GetXaxis()->SetLabelSize(0.045);
    histoEmptyPt->GetXaxis()->SetTitleSize(0.045);
    histoEmptyPt->GetYaxis()->SetRangeUser(0.00001, 0.5);
    histoEmptyPt->Draw();

    TLegend *legPt = new TLegend(0.25, 0.185, 0.5, 0.536);
    legPt->SetFillStyle(0);
    legPt->SetLineColorAlpha(0, 0);
    legPt->SetTextColor(kBlack);
    legPt->SetMargin(0.1);
    legPt->AddEntry("NULL", Form("Iteration %d", iIteration), "");

    //For each multiplicity get the rapidity and pt shapes
    for (int iMultBin = 0; iMultBin < numberOfMultRanges; iMultBin++)
    {
      canRapShapes->cd();
      TF1 *fRap = GetRapDistribution(multRanges[iMultBin][0], multRanges[iMultBin][1], iIteration);
      fRap->SetLineColor(GetColorIndex(arrayColors[iMultBin]));
      fRap->SetParameter(0, fRap->GetParameter(0) / fRap->Integral(-4, -2.5));
      fRap->Draw("same");
      legRap->AddEntry(fRap, Form("%d #leq N_{trk}^{corr} #leq %d", multRanges[iMultBin][0], multRanges[iMultBin][1]), "l");

      canPtShapes->cd();
      TF1 *fPt = GetPtDistribution(multRanges[iMultBin][0], multRanges[iMultBin][1], iIteration);
      fPt->SetLineColor(GetColorIndex(arrayColors[iMultBin]));
      fPt->SetParameter(0, fPt->GetParameter(0) / fPt->Integral(0, 20));
      fPt->Draw("same");
      legPt->AddEntry(fPt, Form("%d #leq N_{trk}^{corr} #leq %d", multRanges[iMultBin][0], multRanges[iMultBin][1]), "l");
    }
    canRapShapes->cd();
    legRap->Draw();

    canPtShapes->cd();
    legPt->Draw();

    canRapShapes->SaveAs(Form("FinalPlots/RapShapesVsMult_iter%d.pdf", iIteration));
    canPtShapes->SaveAs(Form("FinalPlots/PtShapesVsMult_iter%d.pdf", iIteration));
  }
}
//--------------------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------------------//
//In the second case, compare for each multiplicity the different input shapes of the different iteration
void CompareIterShapesPerMult()
{

  //Get the last iteration for the first multiplicity bin (for now assume it is the same for all the bins):
  TString strLatestIter = GetLatestIteration(multRanges[0][0], multRanges[0][1]);
  int latestIter = ((TString)strLatestIter(strLatestIter.Length() - 1, strLatestIter.Length() - 1)).Atoi();

  for (int iMultBin = 0; iMultBin < numberOfMultRanges; iMultBin++)
  {

    TCanvas *canRapShapes = new TCanvas(Form("canRapShapes_Mult%dto%d", multRanges[iMultBin][0], multRanges[iMultBin][1]), "", 800, 800);
    SetCanvasStyle(canRapShapes);
    TH1F *histoEmptyRap = new TH1F(Form("histoEmptyRap_Mult%d", iMultBin), "", 10, -4, -2.5);
    SetHistoStyle(histoEmptyRap, kWhite, kFullCircle, 0.8, "#it{y}", "dN/d#it{y}", 1, kFALSE);
    histoEmptyRap->GetXaxis()->SetLabelSize(0.045);
    histoEmptyRap->GetXaxis()->SetTitleSize(0.045);
    histoEmptyRap->GetYaxis()->SetRangeUser(0.35, 1);
    histoEmptyRap->Draw();

    TLegend *legRap = new TLegend(0.639, 0.185, 0.889, 0.536);
    legRap->SetFillStyle(0);
    legRap->SetLineColorAlpha(0, 0);
    legRap->SetTextColor(kBlack);
    legRap->SetMargin(0.1);
    legRap->AddEntry("NULL", Form("%d #leq N_{trk}^{corr} #leq %d", multRanges[iMultBin][0], multRanges[iMultBin][1]), "");

    TCanvas *canPtShapes = new TCanvas(Form("canPtShapes_Mult%d", iMultBin), "", 800, 800);
    SetCanvasStyle(canPtShapes);
    canPtShapes->SetLogy();
    TH1F *histoEmptyPt = new TH1F(Form("histoEmptyPt_Mult%d", iMultBin), "", 10, 0, 20);
    SetHistoStyle(histoEmptyPt, kWhite, kFullCircle, 0.8, "#it{p}_{T}", "dN/d#it{p}_{T}", 1, kFALSE);
    histoEmptyPt->GetXaxis()->SetLabelSize(0.045);
    histoEmptyPt->GetXaxis()->SetTitleSize(0.045);
    histoEmptyPt->GetYaxis()->SetRangeUser(0.00001, 0.5);
    histoEmptyPt->Draw();

    TLegend *legPt = new TLegend(0.25, 0.185, 0.5, 0.536);
    legPt->SetFillStyle(0);
    legPt->SetLineColorAlpha(0, 0);
    legPt->SetTextColor(kBlack);
    legPt->SetMargin(0.1);
    legPt->AddEntry("NULL", Form("%d #leq N_{trk}^{corr} #leq %d", multRanges[iMultBin][0], multRanges[iMultBin][1]), "");

    //For each iteration get the rapidity and pt shapes
    for (int iIteration = 0; iIteration <= latestIter; iIteration++)
    {
      canRapShapes->cd();
      TF1 *fRap = GetRapDistribution(multRanges[iMultBin][0], multRanges[iMultBin][1], iIteration);
      fRap->SetLineColor(GetColorIndex(arrayColors[iIteration]));
      fRap->SetParameter(0, fRap->GetParameter(0) / fRap->Integral(-4, -2.5));
      fRap->Draw("same");
      legRap->AddEntry(fRap, Form("iteration %d",iIteration), "l");

      canPtShapes->cd();
      TF1 *fPt = GetPtDistribution(multRanges[iMultBin][0], multRanges[iMultBin][1], iIteration);
      fPt->SetLineColor(GetColorIndex(arrayColors[iIteration]));
      fPt->SetParameter(0, fPt->GetParameter(0) / fPt->Integral(0, 20));
      fPt->Draw("same");
      legPt->AddEntry(fPt, Form("iteration %d",iIteration), "l");
    }
    canRapShapes->cd();
    legRap->Draw();

    canPtShapes->cd();
    legPt->Draw();

    canRapShapes->SaveAs(Form("FinalPlots/RapShapesVsIter_Mult%dto%d.pdf", multRanges[iMultBin][0], multRanges[iMultBin][1]));
    canPtShapes->SaveAs(Form("FinalPlots/PtShapesVsIter_Mult%dto%d.pdf", multRanges[iMultBin][0], multRanges[iMultBin][1]));
  }
}
//--------------------------------------------------------------------------------------------//
