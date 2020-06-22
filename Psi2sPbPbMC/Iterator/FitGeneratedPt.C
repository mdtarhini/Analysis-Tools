/*
This is a standalone macro and it is not a part of the chain-of-functions called in AccEffiIterator.C. IT is just meant to obtain the pt shape of the generated Psi(2s) with a modified function
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
#include "Common.C"
TString inputFileName = "GridAnalysis/AnalysisResults.root";

enum enumDimuon
{
  kDimuonInvMass,
  kDimuonPt,
  kDimuonRapidity
};

//-------------------------------------------------------------------------------------------------------------------------------------//
void FitGeneratedPt()
{

  //-------------------------------------------------------------------------------------------------------------------------------------//
  Double_t arrayPtBins[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
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
  Float_t centrality;
  tree->SetBranchAddress("GenDimuon", &tempoVectorGenDimuon);
  tree->SetBranchAddress("RecDimuon", &tempoVectorRecDimuon);
  tree->SetBranchAddress("runNumber", &runNumber);
  tree->SetBranchAddress("centrality", &centrality);
  //-------------------------------------------------------------------------------------------------------------------------------------//

  //-------------------------------------------------------------------------------------------------------------------------------------//
  //Histograms for generated
  TH1F *histoGenPt = new TH1F("histoGenPt", "", numberOfPtBins, &arrayPtBins[0]);
  histoGenPt->Sumw2();
  //-------------------------------------------------------------------------------------------------------------------------------------//

  //-------------------------------------------------------------------------------------------------------------------------------------//
  for (Int_t iEvent = 0; iEvent < nEntries; iEvent++)
  {
    if (iEvent % 100 == 0)
      printf("Filling histograms  ... %.0f%%%s", 100. * iEvent / nEntries, (iEvent < nEntries) ? "\r" : "\n");
    tree->GetEntry(iEvent);

    if (tempoVectorGenDimuon->at(kDimuonRapidity) < -2.5 && tempoVectorGenDimuon->at(kDimuonRapidity) > -4 && tempoVectorGenDimuon->at(kDimuonPt) < 12)
    {
      histoGenPt->Fill(tempoVectorGenDimuon->at(kDimuonPt));
    }
  }
  //-------------------------------------------------------------------------------------------------------------------------------------//
  histoGenPt->Draw();
  TF1 *fPtDistribution = new TF1("fPtDistribution", "[0]*x/((1+(x/[1])**2)**[2] )", 0, 12);
  fPtDistribution->SetParameter(0, histoGenPt->GetMaximum());
  fPtDistribution->SetParameter(1, 3.71);
  fPtDistribution->SetParameter(2, 4.47);
  histoGenPt->Fit(fPtDistribution, "RELIS", "", 0, 12);
}
