/*
This macro takes as inputs a tree that contains dimuon information (generated and reconstructed) and fill them in histograms. The Acc*Effi is calculated as well.
This macros has been modified to weight the pt and rapidity shape via an iterative procedure. It also need the multiplicity bin as an argument.
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
#include "TChain.h"
#include "TSystemDirectory.h"
#include "TSystemFile.h"
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
TString inputFilesDir = "../MC/rootFilesPerRun";

enum enumDimuon
{
  kDimuonInvMass,
  kDimuonPt,
  kDimuonRapidity
};

//-------------------------------------------------------------------------------------------------------------------------------------//
void FillHistoAndGetAccEffi(Int_t multMin = 1, Int_t multMax =8, Int_t iteration = 0)
{

  //-------------------------------------------------------------------------------------------------------------------------------------//
  Double_t arrayRapidityBins[] = {-4, -3.83333, -3.5, -3.16667, -2.83333, -2.5};
  int numberOfRapidityBins = sizeof(arrayRapidityBins) / sizeof(arrayRapidityBins[0]) - 1;

  Double_t arrayPtBins[] = {0, 0.5, 1, 1.5, 2, 2.5, 3, 3.5, 4, 4.5, 5, 5.5, 6, 7, 8, 9, 10, 11, 12, 15, 20};
  int numberOfPtBins = sizeof(arrayPtBins) / sizeof(arrayPtBins[0]) - 1;
  //-------------------------------------------------------------------------------------------------------------------------------------//

  //-------------------------------------------------------------------------------------------------------------------------------------//
  //Input file

  //------------------------------------------------------------------------------------------------------//
  TChain *mergedTree = new TChain("eventsTree");
  //Loop over the priods and for each one, access the different .root files and attach them to the chain:

  TSystemDirectory rootFilesDir(inputFilesDir, inputFilesDir);
  TList *listOfFiles = rootFilesDir.GetListOfFiles();
  if (listOfFiles)
  {
    TSystemFile *fileInList;
    TString fileInListName;
    TIter next(listOfFiles);
    while ((fileInList = (TSystemFile *)next()))
    {
      fileInListName = fileInList->GetName();
      if (!fileInList->IsDirectory() && fileInListName.EndsWith(".root"))
      {
        cout << Form("Attaching %s", fileInListName.Data()) << endl;
        TString fileInListPath;
        fileInListPath.Form("%s/%s", inputFilesDir.Data(), fileInListName.Data());
        mergedTree->Add(fileInListPath);
      }
    }
  }
  //end of loop over files in directory

  //------------------------------------------------------------------------------------------------------//

  Int_t nEntries = mergedTree->GetEntries();

  std::vector<double> *tempoVectorGenDimuon = 0;
  std::vector<double> *tempoVectorRecDimuon = 0;
  Int_t runNumber;
  mergedTree->SetBranchAddress("GenDimuon", &tempoVectorGenDimuon);
  mergedTree->SetBranchAddress("RecDimuon", &tempoVectorRecDimuon);
  //-------------------------------------------------------------------------------------------------------------------------------------//

  //-------------------------------------------------------------------------------------------------------------------------------------//
  //Output File
  gSystem->Exec(Form("mkdir -p Mult-%dto%d/AccEffi/iter-%d",multMin,multMax, iteration));
  TFile *outputFile = new TFile(Form("Mult-%dto%d/AccEffi/iter-%d/AccEffiValues.root", multMin,multMax, iteration), "recreate");

  //Histograms for generated
  TH1F *histoGenInvMass = new TH1F("histoGenInvMass", "", 1000, 0, 10);
  histoGenInvMass->Sumw2();

  TH1F *histoGenPt = new TH1F("histoGenPt", "", numberOfPtBins, &arrayPtBins[0]);
  histoGenPt->Sumw2();

  TH1F *histoGenRapidity = new TH1F("histoGenRapidity", "", numberOfRapidityBins, &arrayRapidityBins[0]);
  histoGenRapidity->Sumw2();

  Double_t totalGenWeighted = 0;

  //Histograms for reconstructed
  TH1F *histoRecInvMass = new TH1F("histoRecInvMass", "", 1000, 0, 10);
  histoRecInvMass->Sumw2();

  TH1F *histoRecPt = new TH1F("histoRecPt", "", numberOfPtBins, &arrayPtBins[0]);
  histoRecPt->Sumw2();

  TH1F *histoRecRapidity = new TH1F("histoRecRapidity", "", numberOfRapidityBins, &arrayRapidityBins[0]);
  histoRecRapidity->Sumw2();

  Double_t totalRecWeighted = 0;
  //-------------------------------------------------------------------------------------------------------------------------------------//

  //-------------------------------------------------------------------------------------------------------------------------------------//
  //Pt and y Weight histograms (for iteration step > 0)
  TH1 *histoPtWeight = NULL;
  TH1 *histoRapWeight = NULL;

  if (iteration > 0)
  {
    histoPtWeight = GetPtWeight(multMin, multMax, iteration, 1);
    histoRapWeight = GetRapWeight(multMin, multMax, iteration, 1);
    for (int iIteration = iteration - 1; iIteration > 0; iIteration--)
    {
      histoPtWeight->Multiply(GetPtWeight(multMin, multMax, iIteration));
      histoRapWeight->Multiply(GetRapWeight(multMin, multMax, iIteration));
    }
  }
  //-------------------------------------------------------------------------------------------------------------------------------------//

  //-------------------------------------------------------------------------------------------------------------------------------------//
  for (Int_t iEvent = 0; iEvent < nEntries; iEvent++)
  {

    if (iEvent % 100 == 0)
      printf("Filling histograms  ... %.0f%%%s", 100. * iEvent / nEntries, (iEvent < nEntries) ? "\r" : "\n");
    mergedTree->GetEntry(iEvent);
    Double_t weightDimuon = 1;
    if (tempoVectorGenDimuon->at(kDimuonRapidity) < -2.5 && tempoVectorGenDimuon->at(kDimuonRapidity) > -4 && tempoVectorGenDimuon->at(kDimuonPt) < 20)
    {
      if (iteration > 0)
      {
        weightDimuon = histoPtWeight->GetBinContent(histoPtWeight->GetXaxis()->FindBin(tempoVectorGenDimuon->at(kDimuonPt))) * histoRapWeight->GetBinContent(histoRapWeight->GetXaxis()->FindBin(tempoVectorGenDimuon->at(kDimuonRapidity)));
      }

      histoGenInvMass->Fill(tempoVectorGenDimuon->at(kDimuonInvMass), 1. / weightDimuon);
      histoGenPt->Fill(tempoVectorGenDimuon->at(kDimuonPt), 1. / weightDimuon);
      histoGenRapidity->Fill(tempoVectorGenDimuon->at(kDimuonRapidity), 1. / weightDimuon);
      totalGenWeighted = totalGenWeighted + 1. / weightDimuon;
    }
    if (tempoVectorRecDimuon->size() > 0 && weightDimuon > 0)
    {
      if (tempoVectorRecDimuon->at(kDimuonRapidity) < -2.5 && tempoVectorRecDimuon->at(kDimuonRapidity) > -4)
      {
        histoRecInvMass->Fill(tempoVectorRecDimuon->at(kDimuonInvMass), 1. / weightDimuon);
        histoRecPt->Fill(tempoVectorRecDimuon->at(kDimuonPt), 1. / weightDimuon);
        histoRecRapidity->Fill(tempoVectorRecDimuon->at(kDimuonRapidity), 1. / weightDimuon);

        if (tempoVectorRecDimuon->at(kDimuonPt) < 20)
        {
          totalRecWeighted = totalRecWeighted + 1. / weightDimuon;
        }
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

  cout << Form("AccEffi = %4.4f", totalRecWeighted / totalGenWeighted) << endl;
  //-------------------------------------------------------------------------------------------------------------------------------------//

  outputFile->Write();
  outputFile->Close();
}
