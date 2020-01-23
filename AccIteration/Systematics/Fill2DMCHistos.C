/*
This macro takes as inputs a tree that contains dimuon information (generated and reconstructed) and fill them in histograms. 
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
TString inputFileName = "../GridAnalysis/AnalysisResults.root";

enum enumDimuon
{
  kDimuonInvMass,
  kDimuonPt,
  kDimuonRapidity
};

//-------------------------------------------------------------------------------------------------------------------------------------//
void Fill2DHistos()
{

  //-------------------------------------------------------------------------------------------------------------------------------------//
  //Not necessarly the bins to be used in the anlysis but rather finer bins to be used for a faster Acc**Effi weighting
  Double_t arrayRapidityBins[] = {-4, -3.875, -3.75, -3.625, -3.5, -3.375, -3.25, -3.125, -3, -2.875, -2.75, -2.625, -2.5};
  int numberOfRapidityBins = sizeof(arrayRapidityBins) / sizeof(arrayRapidityBins[0]) - 1;

  Double_t arrayPtBins[] = {0, 0.5,1,1.5,2,2.5,3,3.5,4,4.5,5,5.5,6,6.5,7,7.5,8,9,10,11,12};
  int numberOfPtBins = sizeof(arrayPtBins) / sizeof(arrayPtBins[0]) - 1;
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
  TFile *outputFile = new TFile("MC2D.root", "recreate");

  TH2F *histoGenRapPt = new TH2F("histoGenRapPt", "", numberOfRapidityBins, &arrayRapidityBins[0], numberOfPtBins, &arrayPtBins[0]);
  histoGenRapPt->Sumw2();


  TH2F *histoRecRapPt = new TH2F("histoRecRapPt", "", numberOfRapidityBins, &arrayRapidityBins[0], numberOfPtBins, &arrayPtBins[0]);
  histoRecRapPt->Sumw2();
  //-------------------------------------------------------------------------------------------------------------------------------------//

  //-------------------------------------------------------------------------------------------------------------------------------------//
  for (Int_t iEvent = 0; iEvent < nEntries; iEvent++)
  {
    if (iEvent % 100 == 0)
      printf("Filling histograms  ... %.0f%%%s", 100. * iEvent / nEntries, (iEvent < nEntries) ? "\r" : "\n");
    tree->GetEntry(iEvent);

    if (tempoVectorGenDimuon->at(kDimuonRapidity) < -2.5 && tempoVectorGenDimuon->at(kDimuonRapidity) > -4 && tempoVectorGenDimuon->at(kDimuonPt) < 12)
    {
      histoGenRapPt->Fill(tempoVectorGenDimuon->at(kDimuonRapidity),tempoVectorGenDimuon->at(kDimuonPt));
    }
    if (tempoVectorRecDimuon->size() > 0)
    {
      if (tempoVectorRecDimuon->at(kDimuonRapidity) < -2.5 && tempoVectorRecDimuon->at(kDimuonRapidity) > -4)
      {

         histoRecRapPt->Fill(tempoVectorRecDimuon->at(kDimuonRapidity),tempoVectorRecDimuon->at(kDimuonPt));
      }
    }
  }
  //-------------------------------------------------------------------------------------------------------------------------------------//
  
  
  outputFile->Write();
  outputFile->Close();
}
