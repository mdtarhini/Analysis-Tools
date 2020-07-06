#include "FitOneHisto.C"

//######################Functions########################//
void FitAndStore();//Main function. It loops over the range (then over the fit configs). Input file and histogram structure should be edited accordingly (around Lines.40-60)
void SetFitResults(TH3F *histoFitResults, TString rangeName, TString testName, std::vector<double> fitResults, std::vector<double> fitResults_err);//Set for each range the fit results for the different fits. 
//#######################################################//

//------------------------Selections-----------------------//
Double_t rapRanges[][2] = {{-4, -2.5}};
int numberOfRapRanges = sizeof(rapRanges) / sizeof(rapRanges[0]);

Double_t centRanges[][2] = {{60, 90}};
int numberOfCentRanges = sizeof(centRanges) / sizeof(centRanges[0]);

Double_t ptRanges[][2] = {{0, 12}};
int numberOfPtRanges = sizeof(ptRanges) / sizeof(ptRanges[0]);
//---------------------------------------------------------//

//------------------Fit configurations---------------------//
Int_t arrayOfBkgdFunctions[] = {kVWG2, kPol2OverPol3};
int numberOfBkgdFunctions = sizeof(arrayOfBkgdFunctions) / sizeof(arrayOfBkgdFunctions[0]);

Int_t arrayOfSigFunctions[] = {kCB21S, kNA601S};
int numberOfSigFunctions = sizeof(arrayOfSigFunctions) / sizeof(arrayOfSigFunctions[0]);

Int_t arrayOfTailsSets[] = {kMCGeant3, kData13TeV};
int numberOfTailsSets = sizeof(arrayOfTailsSets) / sizeof(arrayOfTailsSets[0]);

Double_t arrayOfPsi2sWidth[] = {1.01,1.05};
int numberOfPsi2sWidth = sizeof(arrayOfPsi2sWidth) / sizeof(arrayOfPsi2sWidth[0]);

Double_t arrayOfFitRanges[][2] = {{2.2, 4.5} };
int numberOfFitRanges = sizeof(arrayOfFitRanges) / sizeof(arrayOfFitRanges[0]);
//---------------------------------------------------------//

void FitAndStore()
{
  TFile *inputFile = new TFile("./signalhistos.root");

  TH3F *histoFitResults = GetFitResultsHisto(kTRUE, "histoFitResults");

  for (int iCentBin = 0; iCentBin < numberOfCentRanges; iCentBin++)
  {
    for (int iRapBin = 0; iRapBin < numberOfRapRanges; iRapBin++)
    {
      for (int iPtBin = 0; iPtBin < numberOfPtRanges; iPtBin++)
      {
        TH1D *histoInvmass = ((TH1D *)inputFile->Get(Form("SignalHisto_%gto%g", centRanges[iCentBin][0], centRanges[iCentBin][1])));
        //++++++++++++++++++++++++++++++++++++++++++++++++++++++//
        //Rebining: Don't rebin in the FitOneHisto.C to avoid sequential rebinning
        histoInvmass->Rebin(2);
        //++++++++++++++++++++++++++++++++++++++++++++++++++++++//
        
        TString rangeName;
        rangeName.Form("Cent_%g-%gAndPt_%g-%gAndRap_%gto%g", centRanges[iCentBin][0], centRanges[iCentBin][1], ptRanges[iPtBin][0], ptRanges[iPtBin][1], rapRanges[iRapBin][0], rapRanges[iRapBin][1]);

        TString plotsPath;
        plotsPath.Form("FitPlots/Cent-%gto%g/Pt-%gto%g/Rap-%gto%g", centRanges[iCentBin][0], centRanges[iCentBin][1], ptRanges[iPtBin][0], ptRanges[iPtBin][1], rapRanges[iRapBin][0], rapRanges[iRapBin][1]);

        gSystem->Exec(Form("mkdir -p %s", plotsPath.Data()));

        //<<<<<<<<<<<<<<<<<<<<<Fit per test >>>>>>>>>>>>>>>>>>>>>//
        std::vector<TString> vectorOfTests;
        for (int iBkgd = 0; iBkgd < numberOfBkgdFunctions; iBkgd++)
        {
          for (int iSig = 0; iSig < numberOfSigFunctions; iSig++)
          {
            for (int iTails = 0; iTails < numberOfTailsSets; iTails++)
            {
              std::vector<double> tailsParams = GetTails(arrayOfSigFunctions[iSig], arrayOfTailsSets[iTails], ptRanges[iPtBin][0], ptRanges[iPtBin][1], rapRanges[iRapBin][0], rapRanges[iRapBin][1]);

              if( (arrayOfSigFunctions[iSig] == kNA601S) && (arrayOfTailsSets[iTails] == kData13TeV) ) continue;
              for (int iFitRange = 0; iFitRange < numberOfFitRanges; iFitRange++)
              {
                for (int iPsi2sWidth = 0; iPsi2sWidth < numberOfPsi2sWidth; iPsi2sWidth++)
                {
                  TString testName;
                  testName.Form("%s_%s_%s_%gto%g_width%g", arrayFunctionNames[arrayOfBkgdFunctions[iBkgd]].Data(), arrayFunctionNames[arrayOfSigFunctions[iSig]].Data(), arrayTailsNames[arrayOfTailsSets[iTails]].Data(), arrayOfFitRanges[iFitRange][0], arrayOfFitRanges[iFitRange][1],arrayOfPsi2sWidth[iPsi2sWidth]);
                  vectorOfTests.push_back(testName);

                  std::vector<double> vectorResults, vectorResultsErr;
                  FitOneHisto(Form("%s/%s", plotsPath.Data(), testName.Data()), histoInvmass, arrayOfSigFunctions[iSig], arrayOfPsi2sWidth[iPsi2sWidth], arrayOfTailsSets[iTails], arrayOfBkgdFunctions[iBkgd], arrayOfFitRanges[iFitRange][0], arrayOfFitRanges[iFitRange][1] ,centRanges[iCentBin][0], centRanges[iCentBin][1], ptRanges[iPtBin][0], ptRanges[iPtBin][1], rapRanges[iRapBin][0], rapRanges[iRapBin][1], kTRUE, vectorResults, vectorResultsErr);

                  SetFitResults(histoFitResults, rangeName, testName, vectorResults, vectorResultsErr);
                }
              }
            }
          }
        }
        //<<<<<<<<<<<<<<<<<<<<<------------>>>>>>>>>>>>>>>>>>>>>//

        delete histoInvmass;
      }
    }
  }

  TFile *outputFile = new TFile(outputFileName, "RECREATE");
  histoFitResults->Write();
  outputFile->Close();
}
//-------------------------------------------------------------------------------------------------------------------------------------//



//-------------------------------------------------------------------------------------------------------------------------------------//
void SetFitResults(TH3F *histoFitResults, TString rangeName, TString testName, std::vector<double> fitResults, std::vector<double> fitResults_err)
{

  //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
  int xBinInHistoFitResults = histoFitResults->GetXaxis()->FindFixBin(rangeName.Data());
  if (xBinInHistoFitResults == -1)
  {
    //This used to work with FindLastBinAbove(), no clue on why it is not working! Replace with a loop over the x-axis labels.
    Int_t nonFilledBin;
    for (nonFilledBin = 0; nonFilledBin < maxNumberOfRanges; nonFilledBin++)
    {
      TString strTempoLabel = histoFitResults->GetXaxis()->GetBinLabel(nonFilledBin + 1);
      if (strTempoLabel == "")
      {
        break;
      }
    }
    histoFitResults->GetXaxis()->SetBinLabel(nonFilledBin + 1, rangeName);
    xBinInHistoFitResults = nonFilledBin + 1;
  }
  //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//

  //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
  int yBinInHistoFitResults = histoFitResults->GetYaxis()->FindFixBin(testName.Data());
  if (yBinInHistoFitResults == -1)
  {
    //This used to work with FindLastBinAbove(), no clue on why it is not working! Replace with a loop over the x-axis labels.
    Int_t nonFilledBin;
    for (nonFilledBin = 0; nonFilledBin < maxNumberOfTests; nonFilledBin++)
    {
      TString strTempoLabel = histoFitResults->GetYaxis()->GetBinLabel(nonFilledBin + 1);
      if (strTempoLabel == "")
      {
        break;
      }
    }
    histoFitResults->GetYaxis()->SetBinLabel(nonFilledBin + 1, testName);
    yBinInHistoFitResults = nonFilledBin + 1;
  }
  //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//

  for (Int_t iVariable = 0; iVariable < numberOfFitVariables; iVariable++)
  {
    histoFitResults->SetBinContent(xBinInHistoFitResults, yBinInHistoFitResults, iVariable + 1, fitResults[iVariable]);
    histoFitResults->SetBinError(xBinInHistoFitResults, yBinInHistoFitResults, iVariable + 1, fitResults_err[iVariable]);
  }
}
//-------------------------------------------------------------------------------------------------------------------------------------//