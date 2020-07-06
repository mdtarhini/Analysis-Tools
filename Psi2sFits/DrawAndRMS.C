#include "Common.C"

//######################Functions########################//
void DrawAndRMS();//Main function
void DrawAndRMSForRange(TH3F *histoFitResultsAll, TString rangeName, TString rangeNameForLegend, std::vector<TString> vectorTests, TString plotsPath);//Called by DrawAndRMS for each range. 
void DrawDownAndUpLines(Double_t x0, Double_t x1, Double_t downYAt, Double_t upYAt, int color, int style, TLegend *leg, TString strForLineInLegend);//Dummy function for drawing
void GetFitResults(TH3F *histoFitResults, TString rangeName, TString testName, std::vector<double> &fitResults, std::vector<double> &fitResults_err);//It gets the results already set by FitAndStore.C
//#######################################################//

//------------------------Selections-----------------------//
Double_t rapRanges[][2] = {{-4, -2.5}};
int numberOfRapRanges = sizeof(rapRanges) / sizeof(rapRanges[0]);

Double_t centRanges[][2] = {{0, 90},{0, 10},{10, 20},{20, 40},{40, 60},{60, 90}};
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

Double_t arrayOfFitRanges[][2] = {{2.2, 4.5},{2.4, 4.7} };
int numberOfFitRanges = sizeof(arrayOfFitRanges) / sizeof(arrayOfFitRanges[0]);
//---------------------------------------------------------//


//-----------------------------------------------------------------------------------------------------//
void DrawAndRMS()
{

  TH3F *histoFitResults = GetFitResultsHisto(kFALSE, "histoFitResults");

  for (int iCentBin = 0; iCentBin < numberOfCentRanges; iCentBin++)
  {
    for (int iRapBin = 0; iRapBin < numberOfRapRanges; iRapBin++)
    {
      for (int iPtBin = 0; iPtBin < numberOfPtRanges; iPtBin++)
      {

        TString rangeName;
        rangeName.Form("Cent_%g-%gAndPt_%g-%gAndRap_%gto%g", centRanges[iCentBin][0], centRanges[iCentBin][1], ptRanges[iPtBin][0], ptRanges[iPtBin][1], rapRanges[iRapBin][0], rapRanges[iRapBin][1]);

        TString rangeNameForLegend;
        rangeNameForLegend.Form("%g-%g %%, %g < #it{p}_{T} < %g , %g < #it{y} < %g", centRanges[iCentBin][0], centRanges[iCentBin][1], ptRanges[iPtBin][0], ptRanges[iPtBin][1], rapRanges[iRapBin][0], rapRanges[iRapBin][1]);

        TString plotsPath;
        plotsPath.Form("Systematics/Cent-%gto%g/Pt-%gto%g/Rap-%gto%g", centRanges[iCentBin][0], centRanges[iCentBin][1], ptRanges[iPtBin][0], ptRanges[iPtBin][1], rapRanges[iRapBin][0], rapRanges[iRapBin][1]);

        gSystem->Exec(Form("mkdir -p %s", plotsPath.Data()));

        std::vector<TString> vectorOfTests;
        for (int iBkgd = 0; iBkgd < numberOfBkgdFunctions; iBkgd++)
        {
          for (int iSig = 0; iSig < numberOfSigFunctions; iSig++)
          {
            for (int iTails = 0; iTails < numberOfTailsSets; iTails++)
            {
              if ((arrayOfSigFunctions[iSig] == kNA601S) && (arrayOfTailsSets[iTails] == kData13TeV))
                continue;
              for (int iFitRange = 0; iFitRange < numberOfFitRanges; iFitRange++)
              {
                for (int iPsi2sWidth = 0; iPsi2sWidth < numberOfPsi2sWidth; iPsi2sWidth++)
                {
                  TString testName;
                  testName.Form("%s_%s_%s_%gto%g_width%g", arrayFunctionNames[arrayOfBkgdFunctions[iBkgd]].Data(), arrayFunctionNames[arrayOfSigFunctions[iSig]].Data(), arrayTailsNames[arrayOfTailsSets[iTails]].Data(), arrayOfFitRanges[iFitRange][0], arrayOfFitRanges[iFitRange][1], arrayOfPsi2sWidth[iPsi2sWidth]);
                  vectorOfTests.push_back(testName);
                }
              }
            }
          }
        }
        DrawAndRMSForRange(histoFitResults, rangeName, rangeNameForLegend, vectorOfTests, plotsPath);
      }
    }
  }
}
//-----------------------------------------------------------------------------------------------------//


//-----------------------------------------------------------------------------------------------------//
void DrawDownAndUpLines(Double_t x0, Double_t x1, Double_t downYAt, Double_t upYAt, int color, int style, TLegend *leg, TString strForLineInLegend)
{
  TLine *lineUp = new TLine(x0, upYAt, x1, upYAt);
  lineUp->SetLineColor(color);
  lineUp->SetLineWidth(1);
  lineUp->SetLineStyle(style);
  lineUp->Draw("same");

  TLine *lineDown = new TLine(x0, downYAt, x1, downYAt);
  lineDown->SetLineColor(color);
  lineDown->SetLineWidth(1);
  lineDown->SetLineStyle(style);
  lineDown->Draw("same");

  leg->AddEntry(lineUp, strForLineInLegend, "l");
}
//-----------------------------------------------------------------------------------------------------//


//-----------------------------------------------------------------------------------------------------//
void GetFitResults(TH3F *histoFitResults, TString rangeName, TString testName, std::vector<double> &fitResults, std::vector<double> &fitResults_err)
{
  if (!histoFitResults)
  {
    cout << "Fit Results file not found" << endl;
    return;
  }
  int xBinInHistoFitResults = histoFitResults->GetXaxis()->FindFixBin(rangeName.Data());
  if (xBinInHistoFitResults == -1)
  {
    cout << "Range not found" << endl;
    return;
  }
  int yBinInHistoFitResults = histoFitResults->GetYaxis()->FindFixBin(testName.Data());
  if (yBinInHistoFitResults == -1)
  {
    cout << "test not found" << endl;
    return;
  }

  for (Int_t iVariable = 0; iVariable < numberOfFitVariables; iVariable++)
  {
    fitResults.push_back(histoFitResults->GetBinContent(xBinInHistoFitResults, yBinInHistoFitResults, iVariable + 1));
    fitResults_err.push_back(histoFitResults->GetBinError(xBinInHistoFitResults, yBinInHistoFitResults, iVariable + 1));
  }
}
//-----------------------------------------------------------------------------------------------------//


//-----------------------------------------------------------------------------------------------------//
void DrawAndRMSForRange(TH3F *histoFitResultsAll, TString rangeName, TString rangeNameForLegend, std::vector<TString> vectorTests, TString plotsPath)
{
  ///////////////////////////////////////////
  gStyle->SetOptStat(0);
  gStyle->SetOptTitle(0);
  ///////////////////////////////////////////

  Int_t numberOfTests = (int)vectorTests.size();

  TH1F *histoFitResults[numberOfFitVariables];

  for (int iVariable = 0; iVariable < numberOfFitVariables; iVariable++)
  {
    histoFitResults[iVariable] = new TH1F(Form("histo%s%s", arrayFitVariableNames[iVariable].Data(), rangeName.Data()), "", numberOfTests, 0, numberOfTests);
  }

  for (Int_t iTest = 0; iTest < numberOfTests; iTest++)
  {
    TString testName = vectorTests[iTest];

    std::vector<double> testFitResults, testFitResults_err;
    GetFitResults(histoFitResultsAll, rangeName, testName, testFitResults, testFitResults_err);

    for (int iVariable = 0; iVariable < numberOfFitVariables; iVariable++)
    {
      histoFitResults[iVariable]->SetBinContent(iTest + 1, testFitResults[iVariable]);
      histoFitResults[iVariable]->SetBinError(iTest + 1, testFitResults_err[iVariable]);
      histoFitResults[iVariable]->GetXaxis()->SetBinLabel(iTest + 1, testName);
    }
  }

  //---------------------------------------------------------------------------------------------//
  //Plot per test, per fit variable
  for (int iVariable = 0; iVariable < numberOfFitVariables; iVariable++)
  {
    TCanvas *canFitResult = new TCanvas(Form("canFitResult_%s_%s", rangeName.Data(), arrayFitVariableNames[iVariable].Data()), "", 1200, 500);
    canFitResult->SetBottomMargin(0.25);

    Double_t variableMean = 0;
    Double_t variable_stat_err = 0;
    Double_t variable_sys_err = 0;
    Double_t numberOfWeights = 0;

    for (int iTest = 1; iTest <= numberOfTests; iTest++)
    {
      if((histoFitResults[kChi2PerTest]->GetBinContent(iTest) > 3) || (histoFitResults[kCovQuality]->GetBinContent(iTest) != 3) || (histoFitResults[kFitStatus]->GetBinContent(iTest) != 0) ) continue;

      Double_t testWeight = 1;
      TString strTestLabel = histoFitResults[iVariable]->GetXaxis()->GetBinLabel(iTest);
      if (strTestLabel.Contains("Data"))
        testWeight = 2;
      variableMean += testWeight * histoFitResults[iVariable]->GetBinContent(iTest);
      variable_stat_err += testWeight * histoFitResults[iVariable]->GetBinError(iTest);
      numberOfWeights += testWeight;
    }

    variableMean /= numberOfWeights;
    variable_stat_err /= numberOfWeights;

    for (int iTest = 1; iTest <= numberOfTests; iTest++)
    {

      if((histoFitResults[kChi2PerTest]->GetBinContent(iTest) > 3) || (histoFitResults[kCovQuality]->GetBinContent(iTest) != 3) || (histoFitResults[kFitStatus]->GetBinContent(iTest) != 0) ) continue;

      Double_t testWeight = 1;
      TString strTestLabel = histoFitResults[iVariable]->GetXaxis()->GetBinLabel(iTest);
      if (strTestLabel.Contains("Data"))
        testWeight = 2;
      variable_sys_err += testWeight * TMath::Power((histoFitResults[iVariable]->GetBinContent(iTest) - variableMean), 2);
    }
    variable_sys_err /= numberOfWeights;
    variable_sys_err = TMath::Sqrt(variable_sys_err);

    histoFitResults[iVariable]->SetMinimum(variableMean - 10 * variable_sys_err);
    histoFitResults[iVariable]->SetMaximum(variableMean + 10 * variable_sys_err);
    histoFitResults[iVariable]->SetMarkerStyle(20);
    histoFitResults[iVariable]->SetMarkerColor(kBlue);
    histoFitResults[iVariable]->SetLineColor(kBlack);
    histoFitResults[iVariable]->GetYaxis()->SetTitle(arrayFitVariableAxisTitles[iVariable].Data());
    histoFitResults[iVariable]->GetYaxis()->SetTitleSize(0.05);
    histoFitResults[iVariable]->GetYaxis()->SetTitleOffset(0.7);
    histoFitResults[iVariable]->GetYaxis()->SetLabelSize(0.035);
    histoFitResults[iVariable]->Draw("p");

    TLine *lineAverage = new TLine(0, variableMean, numberOfTests, variableMean);
    lineAverage->SetLineColor(kBlue);
    lineAverage->SetLineWidth(1);
    lineAverage->Draw("");

    TLegend *legMain = new TLegend(0.1, 0.70, 0.4, 0.84);
    legMain->SetFillStyle(0);
    legMain->SetLineColorAlpha(0, 0);
    legMain->SetTextColor(kBlack);
    legMain->SetMargin(0.1);
    legMain->AddEntry("NULL", Form("%s", rangeNameForLegend.Data()), "");
    legMain->Draw();

    if (arrayFitVariableIsQuantity[iVariable])
    {
      TLegend *legRMSLines = new TLegend(0.142, 0.058, 0.85, 0.20);
      legRMSLines->SetFillStyle(0);
      legRMSLines->SetLineColorAlpha(0, 0);
      legRMSLines->SetTextColor(kBlack);
      legRMSLines->SetMargin(0.1);
      DrawDownAndUpLines(0, numberOfTests, variableMean - variable_sys_err, variableMean + variable_sys_err, kRed, 2, legRMSLines, "#pm RMS");
      DrawDownAndUpLines(0, numberOfTests, variableMean - 2 * variable_sys_err, variableMean + 2 * variable_sys_err, kGreen+2, 5, legRMSLines, "#pm 2*RMS");

      if (arrayFitVariableAxisTitles[iVariable].Contains("N_"))
      {

        legMain->AddEntry("NULL", Form("%s = %d #pm %d (stat) #pm %d (sys) (%2.2f %%)", arrayFitVariableAxisTitles[iVariable].Data(), (int)variableMean, (int)variable_stat_err, (int)variable_sys_err, 100 * variable_sys_err / variableMean), "");
        legMain->Draw();
      }
    }

    canFitResult->SaveAs(Form("%s/%s.pdf", plotsPath.Data(), arrayFitVariableNames[iVariable].Data()));
    // delete canFitResult;
  }
}
//-----------------------------------------------------------------------------------------------------//
