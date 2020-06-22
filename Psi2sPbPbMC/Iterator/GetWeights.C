/*
This macro return the weight as a function of y for a given iteration step. The weight is given by the ratio of the normalised y fit from the current step and the one before (for the step-0 take the generated input shape).
The macro is basically the one sent by Laure (weight.C)
*/
#include "../Common/Common.C"
//--------------------------------------------------------------------------------------------//
//Define the two rapidity distributions and the shape descrirapions
TF1 *rapDistributionOld = NULL;
TF1 *rapDistributionNew = NULL;
TF1 *rapDistributionNewScaled = NULL;
TF1 *rapWeight = NULL;
Double_t rapIntegralOld = 0;
Double_t rapIntegralNew = 0;

Double_t RescaleRap(Double_t *x, Double_t *par)
{
  const Double_t xx = x[0];
  return (rapDistributionNew->Eval(x[0]) * rapIntegralOld / rapIntegralNew);
}

Double_t rapdist(Double_t *x, Double_t *par)
{
  return (par[0] * TMath::Exp(-0.5 * TMath::Power(x[0] / par[1], 2)));
}

Double_t DivideRap(Double_t *x, Double_t *par)
{
  return (rapDistributionNewScaled->Eval(x[0])) / (rapDistributionOld->Eval(x[0]));
}
//--------------------------------------------------------------------------------------------//

//--------------------------------------------------------------------------------------------//
//Simle funtion to get a TF1 from a given step
TF1 *GetRapDistribution(Int_t iteration = 1)
{
  TString inputData = gSystem->GetFromPipe(Form("cat ../Iterator/RapShapeIterations/iter-%d/values.txt", iteration));
  TObjArray *objInputData = inputData.Tokenize("\n");

  TF1 *rapDistribution = new TF1(Form("RapDistribution_%d", iteration), rapdist, -4, -2.5, 2);
  rapDistribution->FixParameter(0, ((TObjString *)objInputData->UncheckedAt(0))->String().Atof());
  rapDistribution->FixParameter(1, ((TObjString *)objInputData->UncheckedAt(2))->String().Atof());
  return rapDistribution;
}
//--------------------------------------------------------------------------------------------//

//--------------------------------------------------------------------------------------------//
//The main function. It return a histogram with the weight values (TODO: return the weight as TF1 without converting it to TH1. This has shown some problems when used for weighting)
TH1 *GetRapWeight(Int_t iteration = 1, Bool_t drawIt = kFALSE)
{

  if (iteration < 1)
  {
    cout << "This macro needs at least one previous iteration." << endl;
    return NULL;
  }

  rapDistributionOld = GetRapDistribution(iteration - 1);
  rapIntegralOld = rapDistributionOld->Integral(-4, -2.5);

  rapDistributionNew = GetRapDistribution(iteration);
  rapIntegralNew = rapDistributionNew->Integral(-4, -2.5);

  rapDistributionNewScaled = new TF1("rapgenrescaled", RescaleRap, -4, -2.5, 0);
  rapWeight = new TF1("rapDivide", DivideRap, -4, -2.5, 0);

  TH1 *histo = rapWeight->CreateHistogram();

  if (drawIt)
  {
    TCanvas *canWeightRap = new TCanvas(Form("canWeightRap_%d",iteration), "", 1020, 800);
    SetCanvasStyle(canWeightRap);
    canWeightRap->SetLogy();

    TPad *padMain, *padRatio;
    padMain = new TPad("padMain", "padMain", 0, 0.3, 1, 1, 0);
    padMain->SetBottomMargin(0.);
    padMain->Draw();

    padRatio = new TPad("padRatio", "padRatio", 0, 0, 1, 0.3, 0);
    padRatio->SetBottomMargin(0.4);
    padRatio->SetTopMargin(0.);
    padRatio->Draw();

    padMain->cd();
    gPad->SetLogy();
    rapDistributionNewScaled->Draw();
    rapDistributionOld->Draw("SAME");
    rapDistributionOld->SetLineColor(kBlue);

    TLegend *legMain = new TLegend(0.67, 0.66, 0.89, 0.90);
    legMain->SetMargin(0.1);
    legMain->SetFillStyle(0);
    legMain->SetLineColorAlpha(0, 0);
    legMain->SetTextColor(kBlack);
    legMain->AddEntry(rapDistributionOld, Form("Iteration: Step-%d", iteration - 1), "l");
    legMain->AddEntry(rapDistributionNewScaled, Form("Iteration: Step-%d", iteration), "l");
    legMain->Draw();

    padRatio->cd();
    TH1F *histoEmpty = new TH1F("emrapy", "", 10, -4, -2.5);
    SetHistoStyle(histoEmpty, kBlack, kFullCircle, 0.8, "y", "ratio", 1, kTRUE);
    histoEmpty->GetYaxis()->SetRangeUser(histo->GetMinimum(), histo->GetMaximum());
    histoEmpty->Draw("");
    rapWeight->Draw("same");
    //Draw line at one:
    TLine *lineAtOne = new TLine(histoEmpty->GetXaxis()->GetXmin(), 1, histoEmpty->GetXaxis()->GetXmax(), 1);
    lineAtOne->SetLineStyle(kDashed);
    lineAtOne->Draw();
    canWeightRap->SaveAs(Form("../Iterator/RapShapeIterations/iter-%d/RapWeight.pdf", iteration));
  }
  return histo;
}
//--------------------------------------------------------------------------------------------//
/*
This macro return the weight as a function of pT for a given iteration step. The weight is given by the ratio of the normalised Pt fit from the current step and the one before (for the step-0 take the generated input shape).
The macro is basically the one sent by Laure (weight.C)
*/


//--------------------------------------------------------------------------------------------//
//Define the two pt distributions and the shape descriptions
TF1 *ptDistributionOld = NULL;
TF1 *ptDistributionNew = NULL;
TF1 *ptDistributionNewScaled = NULL;
Double_t ptIntegralOld = 0;
Double_t ptIntegralNew = 0;

Double_t RescalePt(Double_t *x, Double_t *par)
{
  const Double_t xx = x[0];
  return (ptDistributionNew->Eval(x[0]) * ptIntegralOld / ptIntegralNew);
}

Double_t pTdist(Double_t *x, Double_t *par)
{
  return (par[0] * x[0]) / (TMath::Power((1 + TMath::Power(x[0] / par[1], par[2])), par[3]));
}

Double_t DividePt(Double_t *x, Double_t *par)
{
  return (ptDistributionNewScaled->Eval(x[0])) / (ptDistributionOld->Eval(x[0]));
}
//--------------------------------------------------------------------------------------------//

//--------------------------------------------------------------------------------------------//
//Simle funtion to get a TF1 from a given step
TF1 *GetPtDistribution(Int_t iteration = 1)
{
  TString inputData = gSystem->GetFromPipe(Form("cat ../Iterator/PtShapeIterations/iter-%d/values.txt", iteration));
  TObjArray *objInputData = inputData.Tokenize("\n");

  TF1 *ptDistribution = new TF1(Form("PtDistribution_%d", iteration), pTdist, 0, 12, 4);
  for (Int_t iParam = 0; iParam < 4; iParam++)
  {
    ptDistribution->FixParameter(iParam, ((TObjString *)objInputData->UncheckedAt(iParam))->String().Atof());
  }
  return ptDistribution;
}
//--------------------------------------------------------------------------------------------//

//--------------------------------------------------------------------------------------------//
//The main function. It return a histogram with the weight values (TODO: return the weight as TF1 without converting it to TH1. This has shown some problems when used for weighting)
TH1 *GetPtWeight(Int_t iteration = 1, Bool_t drawIt = kFALSE)
{

  ptDistributionOld = GetPtDistribution(iteration - 1);

  ptIntegralOld = ptDistributionOld->Integral(0, 12);

  ptDistributionNew = GetPtDistribution(iteration);
  ptIntegralNew = ptDistributionNew->Integral(0, 12);

  ptDistributionNewScaled = new TF1("pTgenrescaled", RescalePt, 0, 12, 0);
  TF1 *ptWeight = new TF1(Form("ptWeight_%d", iteration), DividePt, 0, 12, 0);
  TH1 *histo = ptWeight->CreateHistogram();

  if (drawIt)
  {
    TCanvas *canWeightPt = new TCanvas(Form("canWeightPt_%d",iteration), "", 1020, 800);
    SetCanvasStyle(canWeightPt);
    canWeightPt->SetLogy();

    TPad *padMain, *padRatio;
    padMain = new TPad("padMain", "padMain", 0, 0.3, 1, 1, 0);
    padMain->SetBottomMargin(0.);
    padMain->Draw();

    padRatio = new TPad("padRatio", "padRatio", 0, 0, 1, 0.3, 0);
    padRatio->SetBottomMargin(0.4);
    padRatio->SetTopMargin(0.);
    padRatio->Draw();

    padMain->cd();
    gPad->SetLogy();
    ptDistributionNewScaled->Draw();
    ptDistributionOld->Draw("SAME");
    ptDistributionOld->SetLineColor(kBlue);

    TLegend *legMain = new TLegend(0.67, 0.66, 0.89, 0.90);
    legMain->SetMargin(0.1);
    legMain->SetFillStyle(0);
    legMain->SetLineColorAlpha(0, 0);
    legMain->SetTextColor(kBlack);
    legMain->AddEntry(ptDistributionOld, Form("Iteration: Step-%d", iteration - 1), "l");
    legMain->AddEntry(ptDistributionNewScaled, Form("Iteration: Step-%d", iteration), "l");
    legMain->Draw();

    padRatio->cd();
    TH1F *histoEmpty = new TH1F("empty", "", 10, 0, 12);
    SetHistoStyle(histoEmpty, kBlack, kFullCircle, 0.8, "pt", "ratio", 1, kTRUE);
    histoEmpty->GetYaxis()->SetRangeUser(histo->GetMinimum(), histo->GetMaximum());
    histoEmpty->Draw("");
    ptWeight->Draw("same");
    //Draw line at one:
    TLine *lineAtOne = new TLine(histoEmpty->GetXaxis()->GetXmin(), 1, histoEmpty->GetXaxis()->GetXmax(), 1);
    lineAtOne->SetLineStyle(kDashed);
    lineAtOne->Draw();
    canWeightPt->SaveAs(Form("../Iterator/PtShapeIterations/iter-%d/PtWeight.pdf", iteration));
  }

  return histo;
}
//--------------------------------------------------------------------------------------------//