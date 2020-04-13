/*
This macro compare the pt and rapidity dependence of the Acc*Eff that correposnds to two different steps A and B.  
*/
//--------------------------------------------------------------------------------------------//
//Print the acc*effi values
TString PrintEffi(TString title, TH1 *histoAccEffi){
  TString strAccEffi = title;
  
  Int_t numberOfBins = histoAccEffi->GetNbinsX();
  for(int iBin=1;iBin<=numberOfBins;iBin++){
    Double_t accEffi = histoAccEffi->GetBinContent(iBin);
    Double_t accEffiErr = histoAccEffi->GetBinError(iBin);

    strAccEffi.Append(Form("\n %g,%g: %4.4f +/- %4.4f \n",histoAccEffi->GetXaxis()->GetBinLowEdge(iBin),histoAccEffi->GetXaxis()->GetBinUpEdge(iBin),accEffi,accEffiErr));
  }
  cout<<strAccEffi<<endl;
  return strAccEffi;
}
//--------------------------------------------------------------------------------------------//



//--------------------------------------------------------------------------------------------//
//Define the two rapidity distributions and the shape descriptions
void CompareIterationsEffi(int multMin = 1, int multMax =8, Int_t iteration_A = 2, Int_t iteration_B = 3)
{

  TFile *inputAccEffFile_A = new TFile(Form("Mult-%dto%d/AccEffi/iter-%d/AccEffiValues.root",multMin, multMax, iteration_A));
  TH1F *histoAccEffiVsPt_A = ((TH1F *)inputAccEffFile_A->Get("histoAccEffiVsPt"));
  PrintEffi(Form("Acc*Effi vs Pt. Iteration step-%d",iteration_A),histoAccEffiVsPt_A);
  TH1F *histoAccEffiVsRap_A = ((TH1F *)inputAccEffFile_A->Get("histoAccEffiVsRap"));
  PrintEffi(Form("Acc*Effi vs rapidity. Iteration step-%d",iteration_A),histoAccEffiVsRap_A);

  TFile *inputAccEffFile_B = new TFile(Form("Mult-%dto%d/AccEffi/iter-%d/AccEffiValues.root", multMin, multMax, iteration_B));
  TH1F *histoAccEffiVsPt_B = ((TH1F *)inputAccEffFile_B->Get("histoAccEffiVsPt"));
  PrintEffi(Form("Acc*Effi vs Pt. Iteration step-%d",iteration_B),histoAccEffiVsPt_B);
  TH1F *histoAccEffiVsRap_B = ((TH1F *)inputAccEffFile_B->Get("histoAccEffiVsRap"));
  PrintEffi(Form("Acc*Effi vs rapidity. Iteration step-%d",iteration_B),histoAccEffiVsRap_B);


  //--------------------------------------------------------------------------------------------//
  //vs rapidity
  TCanvas *canAccEffiVsRap = new TCanvas(Form("canAccEffiVsRap_%dto%d",iteration_A,iteration_B), "", 1020, 800);
  SetCanvasStyle(canAccEffiVsRap);

  TPad *padRapMain, *padRapRatio;
  padRapMain = new TPad("padRapMain", "padRapMain", 0, 0.3, 1, 1, 0);
  padRapMain->SetBottomMargin(0.);
  padRapMain->Draw();

  padRapRatio = new TPad("padRapRatio", "padRapRatio", 0, 0, 1, 0.3, 0);
  padRapRatio->SetBottomMargin(0.4);
  padRapRatio->SetTopMargin(0.);
  padRapRatio->Draw();

  padRapMain->cd();
  SetHistoStyle(histoAccEffiVsRap_A, kBlue, kFullCircle, 0.8, "y", "Acc*Effi", 1, kFALSE);
  histoAccEffiVsRap_A->Draw();
  SetHistoStyle(histoAccEffiVsRap_B, kRed, kFullCircle, 0.8, "y", "Acc*Effi", 1, kFALSE);
  histoAccEffiVsRap_B->Draw("SAME");
  

  TLegend *legRap = new TLegend(0.37, 0.04, 0.59, 0.28);
  legRap->SetMargin(0.1);
  legRap->SetFillStyle(0);
  legRap->SetLineColorAlpha(0, 0);
  legRap->SetTextColor(kBlack);
  legRap->AddEntry(histoAccEffiVsRap_A, Form("Iteration: Step-%d", iteration_A), "epl");
  legRap->AddEntry(histoAccEffiVsRap_B, Form("Iteration: Step-%d", iteration_B), "epl");
  legRap->Draw();

  padRapRatio->cd();
  TH1F *histoRatioVsRap = new TH1F(*histoAccEffiVsRap_B);
  histoRatioVsRap->Divide(histoAccEffiVsRap_A);
  SetHistoStyle(histoRatioVsRap, kBlack, kFullCircle, 0.8, "y", "ratio", 1, kTRUE);
  histoRatioVsRap->Draw();
  //Draw line at one:
  TLine *lineAtOneRap = new TLine(histoRatioVsRap->GetXaxis()->GetXmin(), 1, histoRatioVsRap->GetXaxis()->GetXmax(), 1);
  lineAtOneRap->SetLineStyle(kDashed);
  lineAtOneRap->Draw();
  canAccEffiVsRap->SaveAs(Form("Mult-%dto%d/AccEffi/iter-%d/AccEffVsRap_ComparisonToIter-%d.pdf", multMin, multMax, iteration_B, iteration_A));
  //--------------------------------------------------------------------------------------------//


  //--------------------------------------------------------------------------------------------//
  //vs pt
  TCanvas *canAccEffiVsPt = new TCanvas(Form("canAccEffiVsPt_%dto%d",iteration_A,iteration_B), "", 1020, 800);
  SetCanvasStyle(canAccEffiVsPt);

  TPad *padPtMain, *padPtRatio;
  padPtMain = new TPad("padPtMain", "padPtMain", 0, 0.3, 1, 1, 0);
  padPtMain->SetBottomMargin(0.);
  padPtMain->Draw();

  padPtRatio = new TPad("padPtRatio", "padPtRatio", 0, 0, 1, 0.3, 0);
  padPtRatio->SetBottomMargin(0.4);
  padPtRatio->SetTopMargin(0.);
  padPtRatio->Draw();

  padPtMain->cd();
  SetHistoStyle(histoAccEffiVsPt_A, kBlue, kFullCircle, 0.8, "y", "Acc*Effi", 1, kFALSE);
  histoAccEffiVsPt_A->Draw();
  SetHistoStyle(histoAccEffiVsPt_B, kRed, kFullCircle, 0.8, "y", "Acc*Effi", 1, kFALSE);
  histoAccEffiVsPt_B->Draw("SAME");
  

  TLegend *legPt = new TLegend(0.15, 0.61, 0.37, 0.85);
  legPt->SetMargin(0.1);
  legPt->SetFillStyle(0);
  legPt->SetLineColorAlpha(0, 0);
  legPt->SetTextColor(kBlack);
  legPt->AddEntry(histoAccEffiVsPt_A, Form("Iteration: Step-%d", iteration_A), "epl");
  legPt->AddEntry(histoAccEffiVsPt_B, Form("Iteration: Step-%d", iteration_B), "epl");
  legPt->Draw();

  padPtRatio->cd();
  TH1F *histoRatioVsPt = new TH1F(*histoAccEffiVsPt_B);
  histoRatioVsPt->Divide(histoAccEffiVsPt_A);
  SetHistoStyle(histoRatioVsPt, kBlack, kFullCircle, 0.8, "y", "ratio", 1, kTRUE);
  histoRatioVsPt->Draw();
  //Draw line at one:
  TLine *lineAtOnePt = new TLine(histoRatioVsPt->GetXaxis()->GetXmin(), 1, histoRatioVsPt->GetXaxis()->GetXmax(), 1);
  lineAtOnePt->SetLineStyle(kDashed);
  lineAtOnePt->Draw();
  canAccEffiVsPt->SaveAs(Form("Mult-%dto%d/AccEffi/iter-%d/AccEffVsPt_ComparisonToIter-%d.pdf", multMin, multMax, iteration_B, iteration_A));
  //--------------------------------------------------------------------------------------------//

}
