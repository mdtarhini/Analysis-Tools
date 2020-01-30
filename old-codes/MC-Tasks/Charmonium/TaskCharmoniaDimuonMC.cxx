/**************************************************************************
* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
*                                                                        *
* Author: The ALICE Off-line Project.                                    *
* Contributors are mentioned in the code where appropriate.              *
*                                                                        *
* Permission to use, copy, modify and distribute this software and its   *
* documentation strictly for non-commercial purposes is hereby granted   *
* without fee, provided that the above copyright notice appears in all   *
* copies and that both the copyright notice and this permission notice   *
* appear in the supporting documentation. The authors make no claims     *
* about the suitability of this software for any purpose. It is          *
* provided "as is" without express or implied warranty.                  *
**************************************************************************/

//include root libraries
#include <iostream>
#include <TCanvas.h>
#include <TList.h>
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TNtuple.h>
#include <TTree.h>
#include <TChain.h>
#include "TMath.h"
#include <TObjArray.h>
#include "THnSparse.h"
#include <TClonesArray.h>
//include aliroot libraries
#include "AliLog.h"
#include "AliAnalysisManager.h"
#include "AliAODv0.h"
#include "AliVEvent.h"
#include "AliESDEvent.h"
#include "AliAODEvent.h"
#include "AliVVZERO.h"
#include "AliESDVZERO.h"
#include "AliAODVZERO.h"
#include "AliVVertex.h"
#include "AliESDVertex.h"
#include "AliAODVertex.h"
#include "AliAnalysisMuonUtility.h"
#include "AliAnalysisUtils.h"
#include "AliMuonTrackCuts.h"
#include "AliOADBMuonTrackCutsParam.h"
#include "AliCentrality.h"
#include "AliAODDimuon.h"
#include "AliVTrack.h"
#include "AliAODTrack.h"
#include "AliAODTracklets.h"
#include "AliAODHeader.h"
#include "AliESDMuonTrack.h"
#include "AliInputEventHandler.h"
#include "AliMultSelection.h"
#include "AliAODDimuon.h"
#include "TaskCharmoniaDimuonMC.h"
#include "AliAODMCHeader.h"
#include "AliAODMCParticle.h"

ClassImp(TaskCharmoniaDimuonMC)

    //________________________________________________________________________
    TaskCharmoniaDimuonMC::TaskCharmoniaDimuonMC()
    : AliAnalysisTaskSE(),
      fArrayMCParticles(),
      fListOutput(0x0),
      fMuonTrackCuts(0),
      fHnGenerated(0),
      fHnReconstructed(0),
      fFirstRun(0),
      fLastRun(0),
      fHistoEventSelection(0)
{
  // Dummy constructor ALWAYS needed for I/O.
}

//________________________________________________________________________
TaskCharmoniaDimuonMC::TaskCharmoniaDimuonMC(const char *name, int firstRun, int lastRun)
    : AliAnalysisTaskSE(name),
      fArrayMCParticles(),
      fListOutput(0x0),
      fMuonTrackCuts(0),
      fHnGenerated(0),
      fHnReconstructed(0),
      fFirstRun(firstRun),
      fLastRun(lastRun),
      fHistoEventSelection(0)
{
  // Input slot #0 works with a TChain - it is connected to the default input container
  DefineInput(0, TChain::Class());
  // Output slot #1 writes into a TH1 container
  DefineOutput(1, TObjArray::Class()); // for output objarray
}

//________________________________________________________________________
TaskCharmoniaDimuonMC::~TaskCharmoniaDimuonMC()
{
  // Destructor. Clean-up the output list, but not the histograms that are put inside
  // (the list is owner and will clean-up these histograms). Protect in PROOF case.
  if (fListOutput && !AliAnalysisManager::GetAnalysisManager()->IsProofMode())
  {
    delete fListOutput;
  }
}

//________________________________________________________________________
void TaskCharmoniaDimuonMC::NotifyRun()
{
  /// Set run number for cuts
  if (fMuonTrackCuts)
    fMuonTrackCuts->SetRun(fInputHandler);
}

//________________________________________________________________________
void TaskCharmoniaDimuonMC::UserCreateOutputObjects()
{
  //Create the event, single muon, dimuon histgrams, to do that, it is only needed to add the name of the histogram to the corresponding list, it will be created and the bins will be set according to the name

  //Event histograms
  fListOutput = new TObjArray(2000);
  fListOutput->SetOwner(kTRUE);
  fListOutput->SetName("ListEvent");

  //DiMuon histograms
  Int_t nbinsDiMuon[5] = {2000, 240, 60, fLastRun - fFirstRun, 2}; //mass,pt,rapidity,run,pdg
  Double_t xminDiMuon[5] = {0, 0, -5, (Double_t)fFirstRun, 0}, xmaxDiMuon[5] = {10, 30, -2, (Double_t)fLastRun, 2};

  fHnGenerated = new THnSparseD("fHnGenerated", "", 5, nbinsDiMuon, xminDiMuon, xmaxDiMuon, 1024 * 16);
  fHnGenerated->Sumw2();
  fListOutput->Add(fHnGenerated);

  fHnReconstructed = new THnSparseD("fHnReconstructed", "", 5, nbinsDiMuon, xminDiMuon, xmaxDiMuon, 1024 * 16);
  fHnReconstructed->Sumw2();
  fListOutput->Add(fHnReconstructed);

  fHistoEventSelection = new TH1I("fHistoEventSelection", "", 10, 0, 10);
  fHistoEventSelection->GetXaxis()->SetBinLabel(kAllEvents + 1, "total");
  fHistoEventSelection->GetXaxis()->SetBinLabel(kEventWithJpsi + 1, "With Jpsi");
  fHistoEventSelection->GetXaxis()->SetBinLabel(kEventWithPsi2s + 1, "With Psi2s");
  fHistoEventSelection->Sumw2();
  fListOutput->Add(fHistoEventSelection);

  fMuonTrackCuts = new AliMuonTrackCuts("StandardMuonTrackCuts", "StandardMuonTrackCuts");
  fMuonTrackCuts->SetAllowDefaultParams(kTRUE);
  fMuonTrackCuts->SetFilterMask(AliMuonTrackCuts::kMuEta | AliMuonTrackCuts::kMuThetaAbs | AliMuonTrackCuts::kMuPdca | AliMuonTrackCuts::kMuMatchLpt);
  fMuonTrackCuts->SetIsMC();

  PostData(1, fListOutput);
}

//________________________________________________________________________
void TaskCharmoniaDimuonMC::UserExec(Option_t *)
{

  Float_t muonMass2 = AliAnalysisMuonUtility::MuonMass2();

  AliAODEvent *aod = 0;
  aod = static_cast<AliAODEvent *>(InputEvent());

  if (!aod)
  {
    AliError("ERROR: Could not retrieve ESD or AOD event !!");
    return;
  }

  int runNumber = aod->GetRunNumber();
  fHistoEventSelection->Fill(kAllEvents);

  fArrayMCParticles = dynamic_cast<TClonesArray *>(InputEvent()->FindListObject(AliAODMCParticle::StdBranchName()));

  for (int iParticle = 0; iParticle < fArrayMCParticles->GetEntries(); iParticle++)
  {

    AliAODMCParticle *particle = (AliAODMCParticle *)fArrayMCParticles->At(iParticle);

    if (particle->GetStatus() == 21)
      continue;

    //Jpsi case
    if ((particle->GetPdgCode() == 443))
    {

      Double_t eventArray[5] = {};
      eventArray[0] = particle->GetCalcMass();
      eventArray[1] = particle->Pt();
      eventArray[2] = particle->Y();
      eventArray[3] = (Double_t)runNumber;
      eventArray[4] = 0.0;

      fHnGenerated->Fill(eventArray, 1);

      break;
    }
    else if ((particle->GetPdgCode() == 100443))
    {

      Double_t eventArray[5] = {};
      eventArray[0] = particle->GetCalcMass();
      eventArray[1] = particle->Pt();
      eventArray[2] = particle->Y();
      eventArray[3] = (Double_t)runNumber;
      eventArray[4] = 1.0;

      fHnGenerated->Fill(eventArray, 1);

      break;
    }
  }

  //reconstructed
  TLorentzVector lvRecFirstMuon, lvRecSecondMuon, lvRecDimuon;
  int numberOfTracks = aod->GetNumberOfTracks();

  for (Int_t iFirstMuon = 0; iFirstMuon < numberOfTracks; iFirstMuon++)
  {

    AliVTrack *firstMuon = aod->GetTrack(iFirstMuon);

    if (!fMuonTrackCuts->IsSelected(firstMuon))
      continue;

    int firstMuonIndex = WhichCharmonia(firstMuon->GetLabel());
    if (firstMuonIndex < 0)
      continue;
    for (Int_t iSecondMuon = iFirstMuon + 1; iSecondMuon < numberOfTracks; iSecondMuon++)
    {

      AliVTrack *secondMuon = aod->GetTrack(iSecondMuon);

      if (!fMuonTrackCuts->IsSelected(secondMuon))
        continue;

      int secondMuonIndex = WhichCharmonia(secondMuon->GetLabel());
      if (secondMuonIndex != firstMuonIndex)
        continue;

      Float_t energy = muonMass2 + firstMuon->P() * firstMuon->P();
      energy = TMath::Sqrt(energy);
      lvRecFirstMuon.SetPxPyPzE(firstMuon->Px(), firstMuon->Py(), firstMuon->Pz(), energy);

      energy = muonMass2 + secondMuon->P() * secondMuon->P();
      energy = TMath::Sqrt(energy);
      lvRecSecondMuon.SetPxPyPzE(secondMuon->Px(), secondMuon->Py(), secondMuon->Pz(), energy);

      lvRecDimuon = lvRecFirstMuon + lvRecSecondMuon;

      if (firstMuon->Charge() == secondMuon->Charge())
        continue;

      Double_t eventArray[5] = {};
      eventArray[0] = lvRecDimuon.M();
      eventArray[1] = lvRecDimuon.Pt();
      eventArray[2] = lvRecDimuon.Rapidity();
      eventArray[3] = (Double_t)runNumber;
      eventArray[4] = (Double_t)secondMuonIndex;

      fHnReconstructed->Fill(eventArray, 1);

    }
  }


  PostData(1, fListOutput);
}

//________________________________________________________________________
void TaskCharmoniaDimuonMC::Terminate(Option_t *)
{
  fListOutput = dynamic_cast<TObjArray *>(GetOutputData(1));
  if (!fListOutput)
  {
    AliError("Could not retrieve TObjArray* fListOutput");
    return;
  }
}

Int_t TaskCharmoniaDimuonMC::WhichCharmonia(int muonIndex)
{

  AliAODMCParticle *muonParticle = (AliAODMCParticle *)fArrayMCParticles->At(muonIndex);
  if (muonIndex < 0)
    return -99;
  if ((muonParticle->GetPdgCode() == 13) || (muonParticle->GetPdgCode() == -13))
  {

    int iMother = muonParticle->GetMother();
    while (iMother >= 0)
    {
      AliAODMCParticle *firstMother = (AliAODMCParticle *)fArrayMCParticles->At(iMother);
      int pdgCodeOfFirstMother = firstMother->GetPdgCode();
      if (pdgCodeOfFirstMother == 443)
      {
        return 0;
      }
      if (pdgCodeOfFirstMother == 100443)
      {
        return 1;
      }
      iMother = firstMother->GetMother();
    }
  }
  return -99;
}
