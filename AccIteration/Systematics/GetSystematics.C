/*
root -l GetSystematics.C
*/

#include "../Common.C"
//-------------------------------------------------------------------------------------------------------------------------------------//
//Input files
TFile *fileMC = new TFile("MC2D.root");
TH2F *histoGenRapPt = ((TH2F *)fileMC->Get("histoGenRapPt"));
TH2F *histoRecRapPt = ((TH2F *)fileMC->Get("histoRecRapPt"));

TFile *fileWeight = new TFile("Weight2D.root");
int numberOfTrials = 25;
//-------------------------------------------------------------------------------------------------------------------------------------//

//-------------------------------------------------------------------------------------------------------------------------------------//
Double_t arrayRapBins[] = {-4, -3.75, -3.5, -3.25, -3, -2.75, -2.5};
int numberOfRapBins = sizeof(arrayRapBins) / sizeof(arrayRapBins[0]) - 1;

Double_t arrayPtBins[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 12};
int numberOfPtBins = sizeof(arrayPtBins) / sizeof(arrayPtBins[0]) - 1;
//-------------------------------------------------------------------------------------------------------------------------------------//

//-------------------------------------------------------------------------------------------------------------------------------------//
double GetEffiPerTrial(Double_t rapMin, Double_t rapMax, Double_t ptMin, Double_t ptMax, int iTrialRap, int iTrialPt)
{
  TH2F *histoWeightRapPt = ((TH2F *)fileWeight->Get(Form("histoWeightRap_%d_Pt_%d", iTrialRap, iTrialPt)));
  TH2F *tempoHistoGen = new TH2F(*histoGenRapPt);
  tempoHistoGen->Divide(histoWeightRapPt);

  TH2F *tempoHistoRec = new TH2F(*histoRecRapPt);
  tempoHistoRec->Divide(histoWeightRapPt);

  Int_t rapMinBin = histoGenRapPt->GetXaxis()->FindBin(rapMin);
  Int_t rapMaxBin = histoGenRapPt->GetXaxis()->FindBin(rapMax);

  Int_t ptMinBin = histoGenRapPt->GetYaxis()->FindBin(ptMin);
  Int_t ptMaxBin = histoGenRapPt->GetYaxis()->FindBin(ptMax);

  Double_t accEffi = tempoHistoRec->Integral(rapMinBin, rapMaxBin, ptMinBin, ptMaxBin) / tempoHistoGen->Integral(rapMinBin, rapMaxBin, ptMinBin, ptMaxBin);

  delete tempoHistoRec;
  delete tempoHistoGen;
  delete histoWeightRapPt;
  return accEffi;
}

void GetSystematics()
{

  //-------------------------------------------------------------------------------------------------------------------------------------//
  //Systematic on the integrated case
  TCanvas *canRMSIntegrated = new TCanvas("canRMSIntegrated", "", 1020, 800);
  SetCanvasStyle(canRMSIntegrated);
  TH1F *histoRMSIntegrated = new TH1F("histoRMSIntegrated", "", 10000, 0, 1);
  for (int iTrialRap = 0; iTrialRap < numberOfTrials; iTrialRap++)
  {
    for (int iTrialPt = 0; iTrialPt < numberOfTrials; iTrialPt++)
    {
      histoRMSIntegrated->Fill(GetEffiPerTrial(-4, -2.5, 0, 12, iTrialRap, iTrialPt));
    }
  }
  histoRMSIntegrated->Draw();
  cout << "integrated systematic = " << 100 * histoRMSIntegrated->GetRMS() / histoRMSIntegrated->GetMean() << endl;
  //-------------------------------------------------------------------------------------------------------------------------------------//

  //-------------------------------------------------------------------------------------------------------------------------------------//
  //Vs Pt
  TCanvas *canRMSVsPt = new TCanvas("canRMSVsPt", "", 1020, 800);
  SetCanvasStyle(canRMSVsPt);
  canRMSVsPt->cd();

  TH1F *histoAccRMSVsPt = new TH1F("histoAccRMSVsPt", "", numberOfPtBins, &arrayPtBins[0]);
  histoAccRMSVsPt->Sumw2();

  for (int iPtBin = 0; iPtBin < numberOfPtBins; iPtBin++)
  {
    TH1F *histoRMS = new TH1F(Form("histoRMSPt_%gto%g", arrayPtBins[iPtBin], arrayPtBins[iPtBin + 1]), "", 10000, 0, 1);
    for (int iTrialRap = 0; iTrialRap < numberOfTrials; iTrialRap++)
    {
      for (int iTrialPt = 0; iTrialPt < numberOfTrials; iTrialPt++)
      {
        histoRMS->Fill(GetEffiPerTrial(-4, -2.5, arrayPtBins[iPtBin], arrayPtBins[iPtBin + 1], iTrialRap, iTrialPt));
      }
    }
    histoAccRMSVsPt->SetBinContent(iPtBin + 1, 100 * histoRMS->GetRMS() / histoRMS->GetMean());
  }
  histoAccRMSVsPt->Draw();
  //-------------------------------------------------------------------------------------------------------------------------------------//

  //-------------------------------------------------------------------------------------------------------------------------------------//
  //Vs Rap
  TCanvas *canRMSVsRap = new TCanvas("canRMSVsRap", "", 1020, 800);
  SetCanvasStyle(canRMSVsRap);
  canRMSVsRap->cd();

  TH1F *histoAccRMSVsRap = new TH1F("histoAccRMSVsRap", "", numberOfRapBins, &arrayRapBins[0]);
  histoAccRMSVsRap->Sumw2();

  for (int iRapBin = 0; iRapBin < numberOfRapBins; iRapBin++)
  {
    TH1F *histoRMS = new TH1F(Form("histoRMSRap_%d", iRapBin), "", 10000, 0, 1);
    for (int iTrialRap = 0; iTrialRap < numberOfTrials; iTrialRap++)
    {
      for (int iTrialPt = 0; iTrialPt < numberOfTrials; iTrialPt++)
      {
        histoRMS->Fill(GetEffiPerTrial(arrayRapBins[iRapBin], arrayRapBins[iRapBin + 1], 0, 12, iTrialRap, iTrialPt));
      }
    }
    histoAccRMSVsRap->SetBinContent(iRapBin + 1, 100 * histoRMS->GetRMS() / histoRMS->GetMean());
  }
  histoAccRMSVsRap->Draw();
  //-------------------------------------------------------------------------------------------------------------------------------------//
}
