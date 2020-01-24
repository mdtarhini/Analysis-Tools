/*
Produce a 2D histogram containing the product of pt and rapidity shaoe varations (results of FitCharmoniaRatio.C).
root -l Fill2DWeights.C
*/
int numberOfTrials = 50;
//-------------------------------------------------------------------------------------------------------------------------------------//
void Fill2DWeights()
{

  //-------------------------------------------------------------------------------------------------------------------------------------//
  Double_t arrayRapidityBins[] = {-4, -3.875, -3.75, -3.625, -3.5, -3.375, -3.25, -3.125, -3, -2.875, -2.75, -2.625, -2.5};
  int numberOfRapidityBins = sizeof(arrayRapidityBins) / sizeof(arrayRapidityBins[0]) - 1;

  Double_t arrayPtBins[] = {0, 0.5, 1, 1.5, 2, 2.5, 3, 3.5, 4, 4.5, 5, 5.5, 6, 6.5, 7, 7.5, 8, 9, 10, 11, 12};
  int numberOfPtBins = sizeof(arrayPtBins) / sizeof(arrayPtBins[0]) - 1;

  //-------------------------------------------------------------------------------------------------------------------------------------//
  //Input files
  TFile *fileRapShapes = new TFile("RapidityShapes.root");
  TFile *filePtShapes = new TFile("PtShapes.root");
  //-------------------------------------------------------------------------------------------------------------------------------------//

  //-------------------------------------------------------------------------------------------------------------------------------------//
  //Output File
  TFile *outputFile = new TFile("Weight2D.root", "recreate");

  for (int iTrialRap = 0; iTrialRap < numberOfTrials; iTrialRap++)
  {
    TF1 *fRap = ((TF1 *)fileRapShapes->Get(Form("RatioVsRap_%d",iTrialRap)));
    for (int iTrialPt = 0; iTrialPt < numberOfTrials; iTrialPt++)
    {
      TF1 *fPt = ((TF1 *)filePtShapes->Get(Form("RatioVsPt_%d",iTrialPt)));
      TH2F *histoWeightRapPt = new TH2F(Form("histoWeightRap_%d_Pt_%d", iTrialRap, iTrialPt), "", numberOfRapidityBins, &arrayRapidityBins[0], numberOfPtBins, &arrayPtBins[0]);
      histoWeightRapPt->Sumw2();

      for (int iRapBin = 0; iRapBin < numberOfRapidityBins; iRapBin++)
      {
        for (int iPtBin = 0; iPtBin < numberOfPtBins; iPtBin++)
        {
          histoWeightRapPt->SetBinContent(iRapBin+1,iPtBin+1,fRap->Eval(histoWeightRapPt->GetXaxis()->GetBinCenter(iRapBin+1))*fPt->Eval(histoWeightRapPt->GetYaxis()->GetBinCenter(iPtBin+1)));
        }
      }
      delete fPt;
    }
    delete fRap;
  }

  outputFile->Write();
  outputFile->Close();
}
