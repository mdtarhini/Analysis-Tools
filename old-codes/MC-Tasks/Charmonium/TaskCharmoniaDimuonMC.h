/*
 *simple DIMuon Analysis task
 *
 */
#ifndef TASKCHARMONIADIMUONMC_H
#define TASKCHARMONIADIMUONMC_H

class TH1F;
class TH2F;
class TH3F;
class TNtuple;
class TTree;
class TList;
class TObjArray;
class TArrayF;
class TClonesArray;

class AliAnalysisManager;
class AliVEvent;
class AliMuonTrackCuts;
class AliAnalysisUtils;

#ifndef ALIANALYSISTASKSE_H
#include "AliAnalysisTaskSE.h"
#endif

class TaskCharmoniaDimuonMC : public AliAnalysisTaskSE
{
public:
  TaskCharmoniaDimuonMC();
  TaskCharmoniaDimuonMC(const char *name, int firstRun, int lastRun);
  virtual ~TaskCharmoniaDimuonMC();
  virtual void NotifyRun();
  virtual void UserCreateOutputObjects();
  virtual void UserExec(Option_t *option);
  virtual void Terminate(Option_t *);
  int WhichCharmonia(int muonIndex);

private:
  AliMuonTrackCuts *fMuonTrackCuts; //!
  Int_t fFirstRun;
  Int_t fLastRun;

  TH1I *fHistoEventSelection;
  enum enumEventSelection
  {
    kAllEvents,
    kEventWithJpsi,
    kEventWithPsi2s
  };
  TClonesArray *fArrayMCParticles; //

  THnSparseD *fHnGenerated;
  THnSparseD *fHnReconstructed;

  TObjArray *fListOutput;

  ClassDef(TaskCharmoniaDimuonMC, 2);
};

#endif
