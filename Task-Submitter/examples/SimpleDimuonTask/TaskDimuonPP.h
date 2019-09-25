/*
 *This is a simple task which can serve as a template for tasks based on dimuon
 *analyses.
 *
 */
#ifndef TASKDIMUONPP_H
#define TASKDIMUONPP_H

//-------------------------------------------------------------------------------------------------//
// needed root classes
class TH1F;
class TH2F;
class TH3F;
class TNtuple;
class TTree;
class TList;
class TObjArray;
class TArrayF;
//-------------------------------------------------------------------------------------------------//

//-------------------------------------------------------------------------------------------------//
// needed Aliroot classes
class AliAnalysisManager;
class AliVEvent;
class AliMuonTrackCuts;
class AliAnalysisUtils;
//-------------------------------------------------------------------------------------------------//

//-------------------------------------------------------------------------------------------------//
// The task inherits from AliAnalysisTaskSE
#ifndef ALIANALYSISTASKSE_H
#include "AliAnalysisTaskSE.h"
#endif
//-------------------------------------------------------------------------------------------------//

//-------------------------------------------------------------------------------------------------//
// Class body
class TaskDimuonPP : public AliAnalysisTaskSE {
  //-----------------------------------------------------------------------------------------------//
  // Add your public functions. UserExec() and UserCreateOutputObjects() are
  // must be. Avoid adding variables here.
public:
  // Default constructor (must)
  TaskDimuonPP();

  // Custom constructor (called once)
  TaskDimuonPP(const char *name, int firstRun, int lastRun);

  // Default destructor
  virtual ~TaskDimuonPP();

  // Useful for setting the run info for some variables like the muontrackcuts
  // object (called once per run)
  virtual void NotifyRun();

  // For histogram, tree, ntuples,.... definition (called once per run)
  virtual void UserCreateOutputObjects();

  // The main function where all the event and tracks selection are applied,
  // histograms and other objects are filled. (will be re-called for each event)
  virtual void UserExec(Option_t *option);

  // To be called at the end
  virtual void Terminate(Option_t *);

  // Example on custom function, will get the correposnding lorentz vector for a
  // muon track
  void TrackToLorentzVector(AliVParticle *muonTrack, TLorentzVector &lvMuon);
  //-----------------------------------------------------------------------------------------------//

  //-----------------------------------------------------------------------------------------------//
  // Private members
private:
  // Very useful for muon track selection (don't remove the '//!' after the ';'
  // which is essential for stremaing options )
  AliMuonTrackCuts *fMuonTrackCuts; //!
  // Some simple variables
  Int_t fFirstRun;
  Int_t fLastRun;

  TH1I *fHistoTotalEventsPerRun;
  TH1I *fHistoCMULEventsPerRun;

  TH1F *fHistoDimuonInvMass;

  TObjArray *fListEvent;

  ClassDef(TaskDimuonPP, 3);
};
//-------------------------------------------------------------------------------------------------//

#endif
