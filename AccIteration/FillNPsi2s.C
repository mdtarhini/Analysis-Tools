/*
Simple Macro to fill the number of Pis(2s) (obtained from Jhuma's 4th version of the analysis note) in histograms. This macro should be called once before the iteration procedure.
*/

//-------------------------------------------------------------------------------------------------------------------------------------//
Double_t arrayRapidityBins[] = {-4, -3.75, -3.5, -3.25, -3, -2.75, -2.5};
int numberOfRapidityBins = sizeof(arrayRapidityBins) / sizeof(arrayRapidityBins[0]) - 1;

Double_t arrayNumberOfPsi2sVsRap[] = {116, 309, 309, 518, 465, 158};
Double_t arrayNumberOfPsi2sVsRap_stat[] = {28, 49, 63, 67, 64, 41};
Double_t arrayNumberOfPsi2sVsRap_sys[] = {16, 27, 30, 35, 43, 20};

Double_t arrayPtBins[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 12};
int numberOfPtBins = sizeof(arrayPtBins) / sizeof(arrayPtBins[0]) - 1;

Double_t arrayNumberOfPsi2sVsPt[] = {192, 539, 626, 228, 228, 103, 68, 41, 87};
Double_t arrayNumberOfPsi2sVsPt_stat[] = {52, 72, 67, 49, 38, 26, 20, 14, 19};
Double_t arrayNumberOfPsi2sVsPt_sys[] = {30, 44, 36, 23, 26, 9, 8, 6, 7};
//-------------------------------------------------------------------------------------------------------------------------------------//

//-------------------------------------------------------------------------------------------------------------------------------------//
void FillNPsi2s()
{
  TFile *outputFile = new TFile("NPsi2s.root", "recreate");

  TH1F *histoNpsi2sVsRapidity = new TH1F("histoNpsi2sVsRapidity", "", numberOfRapidityBins, &arrayRapidityBins[0]);

  for (int iRapidity = 0; iRapidity < numberOfRapidityBins; iRapidity++)
  {
    Double_t rapidityBinWidth = (arrayRapidityBins[iRapidity + 1] - arrayRapidityBins[iRapidity]);
    histoNpsi2sVsRapidity->SetBinContent(iRapidity + 1, (arrayNumberOfPsi2sVsRap[iRapidity]) / rapidityBinWidth);

    Double_t statError = arrayNumberOfPsi2sVsRap_stat[iRapidity] / arrayNumberOfPsi2sVsRap[iRapidity];
    Double_t sysError = arrayNumberOfPsi2sVsRap_sys[iRapidity] / arrayNumberOfPsi2sVsRap[iRapidity];

    Double_t error = TMath::Sqrt(statError * statError + sysError * sysError) * arrayNumberOfPsi2sVsRap[iRapidity];

    histoNpsi2sVsRapidity->SetBinError(iRapidity + 1, error / rapidityBinWidth);
  }

  TH1F *histoNpsi2sVsPt = new TH1F("histoNpsi2sVsPt", "", numberOfPtBins, &arrayPtBins[0]);

  for (int iPt = 0; iPt < numberOfPtBins; iPt++)
  {
    Double_t ptBinWidth = (arrayPtBins[iPt + 1] - arrayPtBins[iPt]);

    histoNpsi2sVsPt->SetBinContent(iPt + 1, arrayNumberOfPsi2sVsPt[iPt] / ptBinWidth);

    Double_t statError = arrayNumberOfPsi2sVsPt_stat[iPt] / arrayNumberOfPsi2sVsPt[iPt];
    Double_t sysError = arrayNumberOfPsi2sVsPt_sys[iPt] / arrayNumberOfPsi2sVsPt[iPt];

    Double_t error = TMath::Sqrt(statError * statError + sysError * sysError) * arrayNumberOfPsi2sVsPt[iPt];

    histoNpsi2sVsPt->SetBinError(iPt + 1, error / ptBinWidth);
  }
  //-------------------------------------------------------------------------------------------------------------------------------------//

  
  outputFile->Write();
  outputFile->Close();
  delete outputFile;
}
