/*
This macro is similar to the one used in the Iterator directory. The only difference is that it is a standalone macro to calculate the Acc*Effi using the results of the last iteration reached. IT also adds the options to get the acc*effi vs centrality and various pt,y selections 
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
#include "../Iterator/GetWeights.C"
TString inputFileName = "../GridAnalysis/AnalysisResults.root";

enum enumDimuon
{
  kDimuonInvMass,
  kDimuonPt,
  kDimuonRapidity
};

TString PrintEffi(TString title, TH1 *histoAccEffi){
  TString strAccEffi = title;
  
  Int_t numberOfBins = histoAccEffi->GetNbinsX();
  for(int iBin=1;iBin<=numberOfBins;iBin++){
    Double_t accEffi = histoAccEffi->GetBinContent(iBin);
    Double_t accEffiErr = histoAccEffi->GetBinError(iBin);

    strAccEffi.Append(Form("\n %s: %4.4f +/- %4.4f \n",histoAccEffi->GetXaxis()->GetBinLabel(iBin),accEffi,accEffiErr));
  }
  cout<<strAccEffi<<endl;
  return strAccEffi;
}

//-------------------------------------------------------------------------------------------------------------------------------------//
void GetAccEffi(Int_t iteration = 1)
{

  //-------------------------------------------------------------------------------------------------------------------------------------//
  Double_t arrayRapidityBins[][2] = {{-4, -3.5}, {-3.5, -3.25}, {-3.25, -3.}, {-3, -2.5}, {-4, -2.5}};
  int numberOfRapidityBins = sizeof(arrayRapidityBins) / sizeof(arrayRapidityBins[0]);

  Double_t arrayPtBins[][2] = {{0, 2}, {2, 3}, {3, 4}, {4, 6}, {6, 12}, {0, 12}};
  int numberOfPtBins = sizeof(arrayPtBins) / sizeof(arrayPtBins[0]);

  Double_t arrayCentBins[][2] = {{0, 20}, {20, 40}, {40, 60}, {60, 90}, {0, 90}};
  int numberOfCentBins = sizeof(arrayCentBins) / sizeof(arrayCentBins[0]);

  //For accEff vs centrality or rapidity
  Double_t ptMin = 0;
  Double_t ptMax = 12;

  //For accEff vs pt or centrality
  Double_t rapMin = -4;
  Double_t rapMax = -2.5;

  //For accEff vs rapidity or Pt
  Double_t centMin = 0;
  Double_t centMax = 90;
  //-------------------------------------------------------------------------------------------------------------------------------------//

  //-------------------------------------------------------------------------------------------------------------------------------------//
  //Input files for CMUL weighting:
  TFile *fileCMULData = new TFile("../DataCMUL/outputEvent.root");
  TH1F *histoCMULEventVsCentrality = ((TH1F *)fileCMULData->Get("histoCMULEventVsCentrality"));
  TH1I *histoCMULEventPerRun = ((TH1I *)fileCMULData->Get("histoCMULEventPerRun"));
  Double_t totalCMULEvents = histoCMULEventVsCentrality->Integral(0, 100);
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
  Float_t centrality;
  tree->SetBranchAddress("GenDimuon", &tempoVectorGenDimuon);
  tree->SetBranchAddress("RecDimuon", &tempoVectorRecDimuon);
  tree->SetBranchAddress("runNumber", &runNumber);
  tree->SetBranchAddress("centrality", &centrality);
  //-------------------------------------------------------------------------------------------------------------------------------------//

  //-------------------------------------------------------------------------------------------------------------------------------------//
  //Output File
  gSystem->Exec(Form("mkdir -p AccEffi/iter-%d", iteration));
  TFile *outputFile = new TFile(Form("AccEffi/iter-%d/AccEffiValues.root", iteration), "recreate");

  //Histograms for generated
  TH1F *histoGenPt = new TH1F("histoGenPt", "", numberOfPtBins, 0, numberOfPtBins);
  histoGenPt->Sumw2();

  TH1F *histoGenRapidity = new TH1F("histoGenRapidity", "", numberOfRapidityBins, 0, numberOfRapidityBins);
  histoGenRapidity->Sumw2();

  TH1F *histoGenCentrality = new TH1F("histoGenCentrality", "", numberOfCentBins, 0, numberOfCentBins);
  histoGenCentrality->Sumw2();

  //Histograms for reconstructed
  TH1F *histoRecPt = new TH1F("histoRecPt", "", numberOfPtBins, 0, numberOfPtBins);
  histoRecPt->Sumw2();
  for (int iPtBin = 0; iPtBin < numberOfPtBins; iPtBin++)
  {
    histoRecPt->GetXaxis()->SetBinLabel(iPtBin + 1, Form("%g-%g", arrayPtBins[iPtBin][0], arrayPtBins[iPtBin][1]));
    histoGenPt->GetXaxis()->SetBinLabel(iPtBin + 1, Form("%g-%g", arrayPtBins[iPtBin][0], arrayPtBins[iPtBin][1]));
  }

  TH1F *histoRecRapidity = new TH1F("histoRecRapidity", "", numberOfRapidityBins, 0, numberOfRapidityBins);
  histoRecRapidity->Sumw2();
  for (int iRapidityBin = 0; iRapidityBin < numberOfRapidityBins; iRapidityBin++)
  {
    histoRecRapidity->GetXaxis()->SetBinLabel(iRapidityBin + 1, Form("%g-%g", -1 * arrayRapidityBins[iRapidityBin][1], -1 * arrayRapidityBins[iRapidityBin][0]));
    histoGenRapidity->GetXaxis()->SetBinLabel(iRapidityBin + 1, Form("%g-%g", -1 * arrayRapidityBins[iRapidityBin][1], -1 * arrayRapidityBins[iRapidityBin][0]));
  }

  TH1F *histoRecCentrality = new TH1F("histoRecCentrality", "", numberOfCentBins, 0, numberOfCentBins);
  histoRecCentrality->Sumw2();
  for (int iCentBin = 0; iCentBin < numberOfCentBins; iCentBin++)
  {
    histoRecCentrality->GetXaxis()->SetBinLabel(iCentBin + 1, Form("%g-%g", arrayCentBins[iCentBin][0], arrayCentBins[iCentBin][1]));
    histoGenCentrality->GetXaxis()->SetBinLabel(iCentBin + 1, Form("%g-%g", arrayCentBins[iCentBin][0], arrayCentBins[iCentBin][1]));
  }
  //-------------------------------------------------------------------------------------------------------------------------------------//

  //-------------------------------------------------------------------------------------------------------------------------------------//
  //Pt and y Weight histograms (for iteration step > 0)
  TH1 *histoPtWeight = NULL;
  TH1 *histoRapWeight = NULL;

  if (iteration > 0)
  {
    histoPtWeight = GetPtWeight(iteration, 0);
    histoRapWeight = GetRapWeight(iteration, 0);
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

    //Weight CMUL per run and CMUL vs centrality
    Double_t cmulFractionPerRun = histoCMULEventPerRun->GetBinContent(histoCMULEventPerRun->GetXaxis()->FindBin(runNumber)) / totalCMULEvents;
    Double_t cmulFractionVsCentrality = histoCMULEventVsCentrality->GetBinContent((int)(centrality / 10.) + 1) / totalCMULEvents;
    weightDimuon = weightDimuon / (cmulFractionPerRun * cmulFractionVsCentrality);
    if (iteration > 0)
    {
      weightDimuon = histoPtWeight->GetBinContent(histoPtWeight->GetXaxis()->FindBin(tempoVectorGenDimuon->at(kDimuonPt))) * histoRapWeight->GetBinContent(histoRapWeight->GetXaxis()->FindBin(tempoVectorGenDimuon->at(kDimuonRapidity)));
    }

    //Fill generated histo vs pt
    if (tempoVectorGenDimuon->at(kDimuonRapidity) > rapMin && tempoVectorGenDimuon->at(kDimuonRapidity) < rapMax && centrality < centMax && centrality > centMin)
    {
      for (int iPtBin = 0; iPtBin < numberOfPtBins; iPtBin++)
      {
        if (tempoVectorGenDimuon->at(kDimuonPt) >= arrayPtBins[iPtBin][0] && tempoVectorGenDimuon->at(kDimuonPt) < arrayPtBins[iPtBin][1])
        {
          histoGenPt->Fill(iPtBin, 1. / weightDimuon);
        }
      }
    }
    //Fill generated histo vs y
    if (tempoVectorGenDimuon->at(kDimuonPt) > ptMin && tempoVectorGenDimuon->at(kDimuonPt) < ptMax && centrality < centMax && centrality > centMin)
    {
      for (int iRapidityBin = 0; iRapidityBin < numberOfRapidityBins; iRapidityBin++)
      {
        if (tempoVectorGenDimuon->at(kDimuonRapidity) >= arrayRapidityBins[iRapidityBin][0] && tempoVectorGenDimuon->at(kDimuonRapidity) < arrayRapidityBins[iRapidityBin][1])
        {
          histoGenRapidity->Fill(iRapidityBin, 1. / weightDimuon);
        }
      }
    }
    //Fill generated histo vs centrality
    if (tempoVectorGenDimuon->at(kDimuonPt) > ptMin && tempoVectorGenDimuon->at(kDimuonPt) < ptMax && tempoVectorGenDimuon->at(kDimuonRapidity) > rapMin && tempoVectorGenDimuon->at(kDimuonRapidity) < rapMax)
    {
      for (int iCentBin = 0; iCentBin < numberOfCentBins; iCentBin++)
      {
        if (centrality >= arrayCentBins[iCentBin][0] && centrality < arrayCentBins[iCentBin][1])
        {
          histoGenCentrality->Fill(iCentBin, 1. / weightDimuon);
        }
      }
    }

    if (tempoVectorRecDimuon->size() > 0 && weightDimuon > 0)
    {
      ///////////////////////////////////////////////////////////////////////
      //Fill generated histo vs pt
      if (tempoVectorRecDimuon->at(kDimuonRapidity) > rapMin && tempoVectorRecDimuon->at(kDimuonRapidity) < rapMax && centrality < centMax && centrality > centMin)
      {
        for (int iPtBin = 0; iPtBin < numberOfPtBins; iPtBin++)
        {
          if (tempoVectorRecDimuon->at(kDimuonPt) >= arrayPtBins[iPtBin][0] && tempoVectorRecDimuon->at(kDimuonPt) < arrayPtBins[iPtBin][1])
          {
            histoRecPt->Fill(iPtBin, 1. / weightDimuon);
          }
        }
      }
      //Fill Recerated histo vs y
      if (tempoVectorRecDimuon->at(kDimuonPt) > ptMin && tempoVectorRecDimuon->at(kDimuonPt) < ptMax && centrality < centMax && centrality > centMin)
      {
        for (int iRapidityBin = 0; iRapidityBin < numberOfRapidityBins; iRapidityBin++)
        {
          if (tempoVectorRecDimuon->at(kDimuonRapidity) >= arrayRapidityBins[iRapidityBin][0] && tempoVectorRecDimuon->at(kDimuonRapidity) < arrayRapidityBins[iRapidityBin][1])
          {
            histoRecRapidity->Fill(iRapidityBin, 1. / weightDimuon);
          }
        }
      }
      //Fill Recerated histo vs centrality
      if (tempoVectorRecDimuon->at(kDimuonPt) > ptMin && tempoVectorRecDimuon->at(kDimuonPt) < ptMax && tempoVectorRecDimuon->at(kDimuonRapidity) > rapMin && tempoVectorRecDimuon->at(kDimuonRapidity) < rapMax)
      {
        for (int iCentBin = 0; iCentBin < numberOfCentBins; iCentBin++)
        {
          if (centrality >= arrayCentBins[iCentBin][0] && centrality < arrayCentBins[iCentBin][1])
          {
            histoRecCentrality->Fill(iCentBin, 1. / weightDimuon);
          }
        }
      }
      ///////////////////////////////////////////////////////////////////////
    }
  }
  //-------------------------------------------------------------------------------------------------------------------------------------//

  //-------------------------------------------------------------------------------------------------------------------------------------//
  //Acc*Effi calculation
  TH1F *histoAccEffiVsPt = new TH1F(*histoRecPt);
  histoAccEffiVsPt->SetName("histoAccEffiVsPt");
  histoAccEffiVsPt->Divide(histoGenPt);
  PrintEffi("Acc*Effi Vs Pt", histoAccEffiVsPt);

  TH1F *histoAccEffiVsRap = new TH1F(*histoRecRapidity);
  histoAccEffiVsRap->SetName("histoAccEffiVsRap");
  histoAccEffiVsRap->Divide(histoGenRapidity);
  PrintEffi("Acc*Effi Vs y", histoAccEffiVsRap);

  TH1F *histoAccEffiVsCent = new TH1F(*histoRecCentrality);
  histoAccEffiVsCent->SetName("histoAccEffiVsCent");
  histoAccEffiVsCent->Divide(histoGenCentrality);
  PrintEffi("Acc*Effi Vs centrality", histoAccEffiVsCent);
  //-------------------------------------------------------------------------------------------------------------------------------------//

  outputFile->Write();
  outputFile->Close();
}
