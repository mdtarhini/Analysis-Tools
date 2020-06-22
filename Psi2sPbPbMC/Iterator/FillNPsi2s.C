/*
Simple Macro to fill the number of Pis(2s) (obtained from Jhuma's 4th version of the analysis note) in histograms. This macro should be called once before the iteration procedure.
*/

//-------------------------------------------------------------------------------------------------------------------------------------//
Double_t arrayRapidityBins[] = {-4, -3.5, -3.25, -3,-2.75,  -2.5};
int numberOfRapidityBins = sizeof(arrayRapidityBins) / sizeof(arrayRapidityBins[0]) - 1;

Double_t arrayNumberOfPsi2sVsRap[] = {2358.2,  1711.8,  2996.27,  2487.87, 869.502};
Double_t arrayNumberOfPsi2sVsRap_stat[] = {710.56,  976.808,  1111.17,  1010.16, 594.847};
Double_t arrayNumberOfPsi2sVsRap_sys[] = {0,0,0,0};

Double_t arrayPtBins[] = {0, 2, 3, 4, 6, 12};
int numberOfPtBins = sizeof(arrayPtBins) / sizeof(arrayPtBins[0]) - 1;

Double_t arrayNumberOfPsi2sVsPt[] = {5035.72,  4138.94,  1432.31,  1207.66,  649.663};
Double_t arrayNumberOfPsi2sVsPt_stat[] = {1457.69,  799.091,  541.61,  427.27,  223.45};
Double_t arrayNumberOfPsi2sVsPt_sys[] = {0,0,0,0,0};


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
