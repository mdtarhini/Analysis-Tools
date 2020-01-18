/*
This macro takes as inputs a tree that contains dimuon information (generated and reconstructed) and fill them in histograms. The Acc*Effi is calculated as well.
This macros has been modified to weight the pt and rapidity shape via an iterative procedure.
*/
#include <vector>
#include "Riostream.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TString.h"
#include "TH1.h"
#include "TH2.h"
#include "TF1.h"
#include "TTree.h"
#include "TObjString.h"
#include "TObjArray.h"
#include "TROOT.h"
#include "TLorentzVector.h"
#include "TMath.h"
#include "TVector3.h"
#include "TSystem.h"
#include "TProfile.h"
#include "TColor.h"
#include "TStyle.h"
#include "TGaxis.h"
#include "TLegend.h"
#include "TString.h"
#include "GetWeights.C"
TString inputFileName = "GridAnalysis/AnalysisResults.root";

enum enumDimuon
{
  kDimuonInvMass,
  kDimuonPt,
  kDimuonRapidity
};



//-------------------------------------------------------------------------------------------------------------------------------------//
void FillHistoAndGetAccEffi(Int_t iteration = 1)
{

  //-------------------------------------------------------------------------------------------------------------------------------------//
  Double_t arrayRapidityBins[] = {-4, -3.75, -3.5, -3.25, -3, -2.75, -2.5};
  int numberOfRapidityBins = sizeof(arrayRapidityBins) / sizeof(arrayRapidityBins[0]) - 1;

  Double_t arrayPtBins[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 12};
  int numberOfPtBins = sizeof(arrayPtBins) / sizeof(arrayPtBins[0]) - 1;
  //-------------------------------------------------------------------------------------------------------------------------------------//

  //-------------------------------------------------------------------------------------------------------------------------------------//
  //Input file
  TFile *fileAnalysed = new TFile(inputFileName.Data());
  TObjArray *obj = ((TObjArray *)fileAnalysed->Get("ListEvent"));
  TTree *tree = ((TTree *)obj->FindObject("eventsTree"));

  Int_t nEntries = tree->GetEntries();

  std::vector<double> *tempoVectorGenDimuon = 0;
  std::vector<double> *tempoVectorRecDimuon = 0;
  Int_t runNumber;
  tree->SetBranchAddress("GenDimuon", &tempoVectorGenDimuon);
  tree->SetBranchAddress("RecDimuon", &tempoVectorRecDimuon);
  tree->SetBranchAddress("runNumber", &runNumber);
  //-------------------------------------------------------------------------------------------------------------------------------------//

  //-------------------------------------------------------------------------------------------------------------------------------------//
  //Output File
  gSystem->Exec(Form("mkdir -p AccEffi/iter-%d", iteration));
  TFile *outputFile = new TFile(Form("AccEffi/iter-%d/AccEffiValues.root", iteration), "recreate");

  //Histograms for generated
  TH1F *histoGenInvMass = new TH1F("histoGenInvMass", "", 1000, 0, 10);
  histoGenInvMass->Sumw2();

  TH1F *histoGenPt = new TH1F("histoGenPt", "", numberOfPtBins, &arrayPtBins[0]);
  histoGenPt->Sumw2();

  TH1F *histoGenRapidity = new TH1F("histoGenRapidity", "", numberOfRapidityBins, &arrayRapidityBins[0]);
  histoGenRapidity->Sumw2();

  //Histograms for reconstructed
  TH1F *histoRecInvMass = new TH1F("histoRecInvMass", "", 1000, 0, 10);
  histoRecInvMass->Sumw2();

  TH1F *histoRecPt = new TH1F("histoRecPt", "", numberOfPtBins, &arrayPtBins[0]);
  histoRecPt->Sumw2();

  TH1F *histoRecRapidity = new TH1F("histoRecRapidity", "", numberOfRapidityBins, &arrayRapidityBins[0]);
  histoRecRapidity->Sumw2();
  //-------------------------------------------------------------------------------------------------------------------------------------//

  //-------------------------------------------------------------------------------------------------------------------------------------//
  //Pt and y Weight histograms (for iteration step > 0)
  TH1 *histoPtWeight=NULL;
  TH1 *histoRapWeight=NULL;

  if (iteration > 0)
  {
    histoPtWeight = GetPtWeight(iteration,1);
    histoRapWeight = GetRapWeight(iteration,1);
    for (int iIteration = iteration - 1; iIteration > 0; iIteration--)
    {
      histoPtWeight->Multiply(GetPtWeight(iIteration));
      histoRapWeight->Multiply(GetRapWeight(iIteration));
    }
  }
  //-------------------------------------------------------------------------------------------------------------------------------------//

  //-------------------------------------------------------------------------------------------------------------------------------------//
  for (Int_t iEvent = 0; iEvent < nEntries; iEvent++)
  {
    if (iEvent % 100 == 0)
      printf("Filling histograms  ... %.0f%%%s", 100. * iEvent / nEntries, (iEvent < nEntries) ? "\r" : "\n");
    tree->GetEntry(iEvent);
    Double_t weightDimuon = 1;
    if (tempoVectorGenDimuon->at(kDimuonRapidity) < -2.5 && tempoVectorGenDimuon->at(kDimuonRapidity) > -4 && tempoVectorGenDimuon->at(kDimuonPt) < 12)
    {
      if (iteration > 0)
      {
        weightDimuon = histoPtWeight->GetBinContent(histoPtWeight->GetXaxis()->FindBin(tempoVectorGenDimuon->at(kDimuonPt))) * histoRapWeight->GetBinContent(histoRapWeight->GetXaxis()->FindBin(tempoVectorGenDimuon->at(kDimuonRapidity)));
      }

      histoGenInvMass->Fill(tempoVectorGenDimuon->at(kDimuonInvMass), 1. / weightDimuon);
      histoGenPt->Fill(tempoVectorGenDimuon->at(kDimuonPt), 1. / weightDimuon);
      histoGenRapidity->Fill(tempoVectorGenDimuon->at(kDimuonRapidity), 1. / weightDimuon);
    }
    if (tempoVectorRecDimuon->size() > 0 && weightDimuon > 0)
    {
      if (tempoVectorRecDimuon->at(kDimuonRapidity) < -2.5 && tempoVectorRecDimuon->at(kDimuonRapidity) > -4)
      {
        histoRecInvMass->Fill(tempoVectorRecDimuon->at(kDimuonInvMass), 1. / weightDimuon);
        histoRecPt->Fill(tempoVectorRecDimuon->at(kDimuonPt), 1. / weightDimuon);
        histoRecRapidity->Fill(tempoVectorRecDimuon->at(kDimuonRapidity), 1. / weightDimuon);
      }
    }
  }
  //-------------------------------------------------------------------------------------------------------------------------------------//

  //-------------------------------------------------------------------------------------------------------------------------------------//
  //Acc*Effi calculation
  TH1F *histoAccEffiVsPt = new TH1F(*histoRecPt);
  histoAccEffiVsPt->SetName("histoAccEffiVsPt");
  histoAccEffiVsPt->Divide(histoGenPt);

  TH1F *histoAccEffiVsRap = new TH1F(*histoRecRapidity);
  histoAccEffiVsRap->SetName("histoAccEffiVsRap");
  histoAccEffiVsRap->Divide(histoGenRapidity);
  //-------------------------------------------------------------------------------------------------------------------------------------//

  outputFile->Write();
  outputFile->Close();
}
