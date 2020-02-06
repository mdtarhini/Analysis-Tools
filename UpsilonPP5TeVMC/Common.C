/*
Basic macro with some functions and flags to be used across diiferent macros
*/

//-------------------------------------------------------------------------------------------------------------------------------------//
void SetHistoStyle(TH1 *histoSomething, Int_t color, Int_t markerStyle, Float_t markerSize, TString xAxisTitle, TString yAxisTitle, Int_t rebinFactor, Bool_t isRatio)
{
  histoSomething->Rebin(rebinFactor);
  histoSomething->SetLineColor(color);
  histoSomething->SetMarkerColor(color);
  histoSomething->SetMarkerStyle(markerStyle);
  histoSomething->SetMarkerSize(markerSize);
  histoSomething->GetXaxis()->SetTitle(xAxisTitle);
  histoSomething->GetYaxis()->SetTitle(yAxisTitle);

  if (!isRatio)
  {
    histoSomething->GetXaxis()->SetLabelSize(0.025);
    histoSomething->GetXaxis()->SetTitleSize(0.035);
    histoSomething->GetXaxis()->SetTitleOffset(1);

    histoSomething->GetYaxis()->SetLabelSize(0.035);
    histoSomething->GetYaxis()->SetTitleSize(0.035);
    histoSomething->GetYaxis()->SetTitleOffset(1.1);
  }
  else{
    histoSomething->GetXaxis()->SetLabelSize(0.09);
    histoSomething->GetXaxis()->SetTitleSize(0.09);
    histoSomething->GetXaxis()->SetTitleOffset(1);

    histoSomething->GetYaxis()->SetLabelSize(0.09);
    histoSomething->GetYaxis()->SetTitleSize(0.09);
    histoSomething->GetYaxis()->SetTitleOffset(0.55);
  }
}
//-------------------------------------------------------------------------------------------------------------------------------------//

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++///
//Google color palette in hex codes
const TString arrayColors[] = {"#d50000", "#4285F4", "#18FFFF", "#FF5722", "#F4B400", "#0F9D58", "#673AB7"};
//-----------------------------------------------------------------------------------------------------------------------------///
//hex to root-int-color
Int_t GetColorIndex(TString strHexIndex = "#000099")
{
  return TColor::GetColor(strHexIndex);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++///

//-------------------------------------------------------------------------------------------------------------------------------------//
void SetCanvasStyle(TCanvas *can)
{
  gStyle->SetOptStat(0);
  gStyle->SetOptTitle(0);
  int font = 42;
  gROOT->SetStyle("Plain");
  gStyle->SetFrameBorderMode(0);
  TGaxis::SetMaxDigits(4);
  gStyle->SetFrameFillColor(0);
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadBorderMode(0);
  gStyle->SetPadColor(10);
  gStyle->SetCanvasColor(10);
  gStyle->SetTitleFillColor(10);
  gStyle->SetTitleBorderSize(1);
  gStyle->SetStatColor(10);
  gStyle->SetStatBorderSize(1);
  gStyle->SetLegendBorderSize(1);
  gStyle->SetDrawBorder(0);
  gStyle->SetTextFont(font);
  gStyle->SetStatFont(font);
  gStyle->SetStatFontSize(0.05);
  gStyle->SetStatX(0.97);
  gStyle->SetStatY(0.98);
  gStyle->SetStatH(0.03);
  gStyle->SetStatW(0.3);
  gStyle->SetTickLength(0.02, "y");
  gStyle->SetLabelSize(0.05, "xyz");
  gStyle->SetLabelFont(font, "xyz");
  gStyle->SetLabelOffset(0.01, "xyz");
  gStyle->SetTitleFont(font, "xyz");
  gStyle->SetTitleOffset(1.1, "xy");
  gStyle->SetTitleSize(0.05, "xyz");
  gStyle->SetMarkerSize(1.3);
  gStyle->SetPalette(1, 0);
  gROOT->ForceStyle();
  gStyle->SetOptStat(0);
  gStyle->SetOptTitle(0);
  gStyle->SetLineWidth(2);
  gStyle->SetLegendFont(42);
  gStyle->SetLegendBorderSize(0);
  gStyle->SetLegendFillColor(10);
  gStyle->SetPadTickY(1);
  gStyle->SetPadTickX(1);
  gStyle->SetEndErrorSize(8);
  can->SetFillColor(0);
  can->SetBorderMode(0);
  can->SetBorderSize(0);
  can->SetLeftMargin(0.18);
  can->SetRightMargin(0.1);
  can->SetBottomMargin(0.152);
  // can->SetTopMargin(0.);
  can->SetFrameBorderMode(0);
}
//-------------------------------------------------------------------------------------------------------------------------------------//