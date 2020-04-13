/*
This macro is to be run for the final comparison of the pt,y integrated Acc*Effi in various multiplicity bins and for different iterations.
*/


#include "Common.C"
using namespace std;
Int_t multRanges[][2] = {{0, 100}, {1, 8}, {9, 14}, {15, 20}, {21, 25}, {26, 33}, {34, 41}, {42, 50}, {51, 60}, {61, 100}};
int numberOfMultRanges = sizeof(multRanges) / sizeof(multRanges[0]);


//--------------------------------------------------------------------------------------------//
std::vector<double> GetIntegratedEffi(TH1 *histoGen, TH1 *histoRec)
{
  //This is a function that uses differential histograms for rec and gen and get the integrated accEffi.

  Int_t numberOfBins = histoGen->GetNbinsX();
  Double_t integralGen = histoGen->Integral(1, numberOfBins);
  Double_t integralRec = histoRec->Integral(1, numberOfBins);
  Double_t accEffi = integralRec / integralGen;
  Double_t accEffi_err = TMath::Sqrt(integralRec) / integralGen;
  std::vector<double> vectorAccEffi;
  vectorAccEffi.push_back(accEffi);
  vectorAccEffi.push_back(accEffi_err);
  return vectorAccEffi;
}
//--------------------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------------------//
//For a given iteration, plot and print the multiplicity dependence of the Acc*Effi
void CompareMultEffiPerItertaion(Int_t iteration, TPad *padMain, TPad *padRatio, TLegend *legMain, int color)
{
  cout << Form("Comparing AccEffi values for iteration %d", iteration) << endl;
  TH1F *histoAccEffiVsMult = new TH1F(Form("histoAccEffiVsMult_%d",iteration), "", numberOfMultRanges, 0, numberOfMultRanges);
  histoAccEffiVsMult->GetYaxis()->SetRangeUser(0.24577,0.27);
  histoAccEffiVsMult->Sumw2();

  TH1F *histoDeltaAccEffiVsMult = new TH1F(Form("histoDeltaAccEffiVsMult_%d",iteration), "", numberOfMultRanges, 0, numberOfMultRanges);
  histoDeltaAccEffiVsMult->GetYaxis()->SetRangeUser(0.951111,1.049999);
  histoDeltaAccEffiVsMult->GetYaxis()->SetNdivisions(505);
  histoDeltaAccEffiVsMult->Sumw2();

  Double_t accEffiReference;
  for (int iMultBin = 0; iMultBin < numberOfMultRanges; iMultBin++)
  {

    //Get the integrated efficiency for this iteration:
    TFile *inputFile = new TFile(Form("Mult-%dto%d/AccEffi/iter-%d/AccEffiValues.root", multRanges[iMultBin][0], multRanges[iMultBin][1], iteration));
    TH1F *histoGen = ((TH1F *)inputFile->Get("histoGenRapidity"));
    TH1F *histoRec = ((TH1F *)inputFile->Get("histoRecRapidity"));
    std::vector<double> vectorAccEffi = GetIntegratedEffi(histoGen, histoRec);
    histoAccEffiVsMult->SetBinContent(iMultBin + 1, vectorAccEffi[0]);
    histoAccEffiVsMult->SetBinError(iMultBin + 1, vectorAccEffi[1]);

    if (iMultBin == 0)
      accEffiReference = vectorAccEffi[0];

    histoDeltaAccEffiVsMult->SetBinContent(iMultBin + 1, vectorAccEffi[0] / accEffiReference);
    histoDeltaAccEffiVsMult->SetBinError(iMultBin + 1, vectorAccEffi[1] / accEffiReference);
    histoDeltaAccEffiVsMult->GetXaxis()->SetBinLabel(iMultBin + 1, Form("%d-%d", multRanges[iMultBin][0], multRanges[iMultBin][1]));

    //Print the information:
    cout << Form("Multiplicity %d-%d: AccEffi = %4.4f +/- %4.4f. (%4.4f * AccEffi(0-100))", multRanges[iMultBin][0], multRanges[iMultBin][1], vectorAccEffi[0], vectorAccEffi[1], vectorAccEffi[0] / accEffiReference) << endl;
  }
  cout << "----------------------------------------------------------------" << endl;

  padMain->cd();
  SetHistoStyle(histoAccEffiVsMult, color, kFullCircle, 0.8, "N_{trk}^{corr}", "A#times#epsilon", 1, kFALSE);
  // histoAccEffiVsMult->GetYaxis()->SetLabelSize()
  histoAccEffiVsMult->Draw(iteration == 0 ? "" : "same");
  legMain->AddEntry(histoAccEffiVsMult, Form("iteration %d",iteration),"p");

  padRatio->cd();
  SetHistoStyle(histoDeltaAccEffiVsMult, color, kFullCircle, 0.8, "N_{trk}^{corr}", "A#times#epsilon/A#times#epsilon(0-100)", 1, kTRUE);
  histoDeltaAccEffiVsMult->Draw(iteration == 0 ? "" : "same");
}
//--------------------------------------------------------------------------------------------//



//--------------------------------------------------------------------------------------------//
//The main function calling the above function for each iteration
void CompareFinalEffi()
{
  

  TCanvas *canAccEffiVsMult = new TCanvas("canAccEffiVsMult", "", 1020, 800);
  SetCanvasStyle(canAccEffiVsMult);

  TPad *padMain, *padRatio;
  padMain = new TPad("padMain", "padMain", 0, 0.3, 1, 1, 0);
  padMain->SetBottomMargin(0.);
  padMain->Draw();

  padRatio = new TPad("padRatio", "padRatio", 0, 0, 1, 0.3, 0);
  padRatio->SetBottomMargin(0.4);
  padRatio->SetTopMargin(0.);
  padRatio->Draw();
  
  TLegend *legMain = new TLegend(0.74, 0.51, 0.99, 0.86);
  legMain->SetFillStyle(0);
  legMain->SetLineColorAlpha(0, 0);
  legMain->SetTextColor(kBlack);
  legMain->SetMargin(0.1);


  //Get the last iteration for the first multiplicity bin (for now assume it is the same for all the bins):
  TString strLatestIter = GetLatestIteration(multRanges[0][0], multRanges[0][1]);
  int latestIter = ((TString)strLatestIter(strLatestIter.Length() - 1, strLatestIter.Length() - 1)).Atoi();

  for(int iIteration = 0;iIteration<=latestIter;iIteration++){
    CompareMultEffiPerItertaion(iIteration, padMain,padRatio, legMain, GetColorIndex(arrayColors[iIteration]));
  }

  padMain->cd();
  legMain->Draw();


  padRatio->cd();
  TLine *lineAtOne = new TLine(0, 1, numberOfMultRanges, 1);
  lineAtOne->SetLineStyle(kDashed);
  lineAtOne->Draw();
  canAccEffiVsMult->SaveAs("FinalPlots/AccEffiVsMult.pdf");

}
//--------------------------------------------------------------------------------------------//