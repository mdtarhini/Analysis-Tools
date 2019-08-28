#if !defined(__CINT__) || defined(__MAKECINT__)
#include <TStyle.h>
#include <TGrid.h>
#include <TCanvas.h>
#include <TH2D.h>
#include <TLine.h>
#include <TView.h>
#include <TMath.h>
#include <TGeoManager.h>
#include "AliITSOnlineCalibrationSPDhandler.h"
#include "AliCDBManager.h"
#include "AliGeomManager.h"
#include "AliITSAlignMille2Module.h"
#endif

//---------------------------------------------------------------------------------------------------//
std::vector<int> runListToVector(TString runListPath, TString splitter)
{
  std::vector<int> vectorRuns;
  TString strRunList = gSystem->GetFromPipe(Form("cat %s", runListPath.Data()));
  TObjArray *objRunList = strRunList.Tokenize(splitter);
  TIter nextRun(objRunList);
  TObjString *strRun;
  while ((strRun = (TObjString *)nextRun()))
  {
    TString currentRun = strRun->GetName();
    int runNumber = currentRun.Atoi();
    vectorRuns.push_back(runNumber);
  }
  return vectorRuns;
}
//---------------------------------------------------------------------------------------------------//


//---------------------------------------------------------------------------------------------------//
void GetSPDActiveLayersPerRun(Int_t runNumber = 274364, TString ocbLocation = "alien://folder=/alice/data/2017/OCDB", TString savePath = "2017/LHC17i")
{
  gStyle->SetOptStat(0);

  AliITSOnlineCalibrationSPDhandler *spdHandler = new AliITSOnlineCalibrationSPDhandler();
  spdHandler->ReadDeadFromDB(runNumber, ocbLocation.Data());
  AliCDBManager::Instance();
  AliCDBManager::Instance()->SetRun(runNumber);
  AliCDBManager::Instance()->SetDefaultStorage(ocbLocation.Data());

  ofstream outputFile(Form("%s/Log_Run_%d.txt", savePath.Data(), runNumber), std::ofstream::out);

  TCanvas *canSPD = new TCanvas("canSPD", "Active Modules ", 800, 1000);
  canSPD->Divide(1, 2);
  canSPD->cd(1);
  gPad->SetGrid();

  TH2D *histoInnerLayers = new TH2D("histoInnerLayers", "Inner layer Active Modules", 4, -2, 2, 20, 0, 20);
  histoInnerLayers->SetXTitle("Z");
  histoInnerLayers->SetYTitle("#varphi");
  histoInnerLayers->GetXaxis()->SetNdivisions(4);
  histoInnerLayers->GetXaxis()->SetLabelColor(kWhite);
  histoInnerLayers->GetYaxis()->SetNdivisions(20);
  histoInnerLayers->GetYaxis()->SetLabelColor(kWhite);

  for (Int_t iModule = 0; iModule < 80; iModule++)
  {
    if ((spdHandler->GetNrBad(iModule)) < 1)
    {
      histoInnerLayers->SetBinContent(4 - (iModule - (iModule / 4) * 4), (iModule / 4) + 1, 1);
      outputFile << "1" << endl;
    }
    else
    {
      histoInnerLayers->SetBinContent(4 - (iModule - (iModule / 4) * 4), (iModule / 4) + 1, 0);
      outputFile << "0" << endl;
    }
  }
  histoInnerLayers->Draw("colz");

  canSPD->cd(2);
  gPad->SetGrid();
  TH2D *histoOuterLayers = new TH2D("histoOuterLayers", "Outer layer Active Modules", 4, -2, 2, 40, 0, 40);
  histoOuterLayers->SetXTitle("Z");
  histoOuterLayers->SetYTitle("#varphi");
  histoOuterLayers->GetXaxis()->SetNdivisions(4);
  histoOuterLayers->GetXaxis()->SetLabelColor(kWhite);
  histoOuterLayers->GetYaxis()->SetNdivisions(40);
  histoOuterLayers->GetYaxis()->SetLabelColor(kWhite);

  for (Int_t iModule = 0; iModule < 160; iModule++)
  {
    if ((spdHandler->GetNrBad(iModule + 80)) < 1)
    {
      histoOuterLayers->SetBinContent(4 - (iModule - (iModule / 4) * 4), (iModule / 4) + 1, 1);
      outputFile << "1" << endl;
    }
    else
    {
      histoOuterLayers->SetBinContent(4 - (iModule - (iModule / 4) * 4), (iModule / 4) + 1, 0);
      outputFile << "0" << endl;
    }
  }
  histoOuterLayers->Draw("colz");
  canSPD->SaveAs(Form("%s/Histograms_Run_%d.pdf", savePath.Data(), runNumber));

  outputFile.close();

  delete canSPD;
  delete histoOuterLayers;
  delete histoInnerLayers;
}
//---------------------------------------------------------------------------------------------------//


//---------------------------------------------------------------------------------------------------//
void GetSPDActiveLayersPerPeriod(TString year, TString period, TString runList)
{

  TGrid::Connect("alien://");
  if (!gGrid)
  {
    printf("no grid connection is available, exiting.\n");
    return;
  }

  gSystem->Exec(Form("mkdir -p %s/%s", year.Data(), period.Data()));

  TString ocdbLocation;
  ocdbLocation.Form("alien://folder=/alice/data/%s/OCDB", year.Data());

  TString outputPath;
  outputPath.Form("%s/%s", year.Data(), period.Data());

  std::vector<int> vectorRunList = runListToVector(runList, "\n");
  Int_t numberOfRunsPeriod = (int)vectorRunList.size();

  for (Int_t iRun = 0; iRun < numberOfRunsPeriod; iRun++)
  {
    GetSPDActiveLayersPerRun(vectorRunList[iRun], ocdbLocation, outputPath);
  }
}
//---------------------------------------------------------------------------------------------------//