/*
In this macro, define the common parameters, such as the signal and back functions index and names.
*/

//--------------------------Enumerations--------------------------//
enum cbTailsParams
{
  kCBAlpha,
  kCBn,
  kCBAlphaPrime,
  kCBnPrime
};
enum fixTailsEnum
{
  kFixNone,
  kFixAll,
  kFixLeft,
  kFixRight,
  kFixAlphas,
  kFixns
};

enum enumFunctions
{
  kCB21S,
  kNA601S,
  kCB22S,
  kNA602S,
  kCB23S,
  kNA603S,
  kVWG,
  kVWG2,
  kPol1OverPol2,
  kPol2OverPol3,
  kExpo,
  kDoubleExpo,
  kExpoPol2
};
enum FitResultVariables
{
  kNJpsiPerTest,
  kNPsi2sPerTest,
  kJpsiMeanPerTest,
  kJpsiSigmaPerTest,
  kChi2PerTest,
  kFitStatus,
  kCovQuality
};

enum enumTails
{
  kData13TeV,
  kMCGeant3,
  kMCGeant4
};

TString  arrayFitVariableNames[] =      {"JpsiNumber",  "Psi2sNumber",  "JpsiMean",   "JpsiSigma",      "ChiSquare",    "FitStatus",  "CovQuality"};
TString arrayFitVariableAxisTitles[] =  {"N_{J/#psi}",  "N_{#psi(2s)}", "M_{J/#psi}", "#sigma_{J/#psi}","#chi^{2}/ndf", "Fit status", "cov-Q"};
Bool_t   arrayFitVariableIsQuantity[] = {kTRUE,         kTRUE,           kTRUE,      kTRUE,       kFALSE,       kFALSE,       kFALSE};
int numberOfFitVariables = sizeof(arrayFitVariableNames) / sizeof(arrayFitVariableNames[0]);

TString arrayFunctionNames[] = {"CB2", "NA60", "CB2", "NA60", "CB2", "NA60", "VWG", "VWG2", "Pol1ToPol2", "Pol2ToPol3", "expo", "2*expo", "expo*Pol2"};

const TString arrayTailsNames[] = {"Data", "MC-GEANT3", "MC-GEANT4"};
std::vector<double> GetTails(Int_t fSig = kCB21S, Int_t fTailsSet = kMCGeant3, Double_t ptMin = 0, Double_t ptMax = 12, Double_t rapMin = -4, Double_t rapMax = -2.5)
{
  std::vector<double> vectorTails;
  if (fSig == kNA601S && fTailsSet == kData13TeV)
  {
    cout << "No data tails for NA60" << endl;
    return vectorTails;
  }

  TString tailBin;
  tailBin.Form("%s_%s_Rap%gto%g_Pt%gto%g", arrayTailsNames[fTailsSet].Data(), arrayFunctionNames[fSig].Data(), -1 * rapMax, -1 * rapMin, ptMin, ptMax);
  cout << tailBin << endl;

  std::map<TString, std::vector<double>> mapOfTails{
      //Geant3-CB2
      {"MC-GEANT3_CB2_Rap2.5to4_Pt0.3to2", {0.88, 2.35, 1.90, 2.45}},
      {"MC-GEANT3_CB2_Rap2.5to4_Pt0to2", {0.88, 2.35, 1.90, 2.45}},
      {"MC-GEANT3_CB2_Rap2.5to4_Pt2to3", {0.88, 2.35, 1.90, 2.45}},
      {"MC-GEANT3_CB2_Rap2.5to4_Pt3to4", {0.88, 2.35, 1.90, 2.45}},
      {"MC-GEANT3_CB2_Rap2.5to4_Pt4to6", {0.88, 2.35, 1.90, 2.45}},
      {"MC-GEANT3_CB2_Rap2.5to4_Pt6to12", {0.88, 2.35, 1.90, 2.45}},
      {"MC-GEANT3_CB2_Rap2.5to4_Pt0.3to12", {0.88, 2.35, 1.90, 2.45}},
      {"MC-GEANT3_CB2_Rap2.5to4_Pt0to12", {0.88, 2.35, 1.90, 2.45}},
      //Data-CB2
      {"Data_CB2_Rap2.5to4_Pt0.3to2", {0.9743, 7.36, 1.84, 16.06}},
      {"Data_CB2_Rap2.5to4_Pt0to2", {0.9743, 7.36, 1.84, 16.06}},
      {"Data_CB2_Rap2.5to4_Pt2to3", {0.9743, 7.36, 1.84, 16.06}},
      {"Data_CB2_Rap2.5to4_Pt3to4", {0.9743, 7.36, 1.84, 16.06}},
      {"Data_CB2_Rap2.5to4_Pt4to6", {0.9743, 7.36, 1.84, 16.06}},
      {"Data_CB2_Rap2.5to4_Pt6to12", {0.9743, 7.36, 1.84, 16.06}},
      {"Data_CB2_Rap2.5to4_Pt0.3to12", {0.9743, 7.36, 1.84, 16.06}},
      {"Data_CB2_Rap2.5to4_Pt0to12", {0.9743, 7.36, 1.84, 16.06}},
      //Geant3-NA60
      {"MC-GEANT3_NA60_Rap2.5to4_Pt0.3to2", {-0.797866, 0.00314949, 0.462584, 0.231261, 2.08121, 0.189483, 1.25368, 0.050364}},
      {"MC-GEANT3_NA60_Rap2.5to4_Pt0to2", {-0.797866, 0.00314949, 0.462584, 0.231261, 2.08121, 0.189483, 1.25368, 0.050364}},
      {"MC-GEANT3_NA60_Rap2.5to4_Pt2to3", {-0.797866, 0.00314949, 0.462584, 0.231261, 2.08121, 0.189483, 1.25368, 0.050364}},
      {"MC-GEANT3_NA60_Rap2.5to4_Pt3to4", {-0.797866, 0.00314949, 0.462584, 0.231261, 2.08121, 0.189483, 1.25368, 0.050364}},
      {"MC-GEANT3_NA60_Rap2.5to4_Pt4to6", {-0.797866, 0.00314949, 0.462584, 0.231261, 2.08121, 0.189483, 1.25368, 0.050364}},
      {"MC-GEANT3_NA60_Rap2.5to4_Pt6to12", {-0.797866, 0.00314949, 0.462584, 0.231261, 2.08121, 0.189483, 1.25368, 0.050364}},
      {"MC-GEANT3_NA60_Rap2.5to4_Pt0.3to12", {-0.797866, 0.00314949, 0.462584, 0.231261, 2.08121, 0.189483, 1.25368, 0.050364}},
      {"MC-GEANT3_NA60_Rap2.5to4_Pt0to12", {-0.797866, 0.00314949, 0.462584, 0.231261, 2.08121, 0.189483, 1.25368, 0.050364}}

  };

  if (mapOfTails.find(tailBin) == mapOfTails.end())
  {
    return vectorTails;
  }
  else
  {
    return mapOfTails[tailBin];
  }
}

//-------------------------------------------------------------------------------------------------------------------------------------//
//Return the histogram which contains the values.
TString outputFileName = "FitResults.root";
const Int_t maxNumberOfRanges = 100;
const Int_t maxNumberOfTests = 100;
TH3F *GetFitResultsHisto(Bool_t createIfNotFound = kFALSE, TString histoName = "histoFitResults")
{
  TH3F *histoFitResults;
  TFile *outputFile = new TFile(outputFileName);
  histoFitResults = ((TH3F *)outputFile->Get("histoFitResults"));

  if (!histoFitResults && createIfNotFound)
  {
    TFile *outputFile = new TFile(outputFileName, "recreate");
    cout << "fit results file not found: creating a new histogram" << endl;
    histoFitResults = new TH3F("histoFitResults", "", maxNumberOfRanges, 0, maxNumberOfRanges, maxNumberOfTests, 0, maxNumberOfTests, numberOfFitVariables, 0, numberOfFitVariables);
  }
  return histoFitResults;
}
//-------------------------------------------------------------------------------------------------------------------------------------//

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++///
//Google color palette in hex codes
std::map<TString, TString> strHexMap{
    {"red", "#d50000"},
    {"blue", "#4285F4"},
    {"cyan", "#18FFFF"},
    {"orange", "#FF5722"},
    {"yellow", "#F4B400"},
    {"green", "#0F9D58"},
    {"purple", "#673AB7"},
    {"pink", "#FFABF5"},
    {"grey", "#616161"},
    {"darkBlue", "#1F2B47"},
    {"redError", "#B00020"}

};
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++///
//text adder
void AddAText(TString text, Double_t x1 = 0.169, Double_t y1 = 0.75, Double_t x2 = 0.464, Double_t y2 = 0.88, int color = kBlack)
{
  TPaveText *textPave = new TPaveText(x1, y1, x2, y2, "brNDC");
  textPave->SetBorderSize(0);
  textPave->SetFillStyle(0);
  textPave->SetLineWidth(2);
  textPave->SetTextAlign(12);
  textPave->SetTextFont(42);
  textPave->SetTextSize(0.03);
  textPave->SetTextColor(color);
  //Split the text for multiLine usage
  TObjArray *objLines = text.Tokenize("\n");
  TIter nextLine(objLines);
  TObjString *strLine;
  while ((strLine=(TObjString*)nextLine())) {
    TString line = strLine->GetName();
    textPave->AddText(line);
  }
  textPave->Draw();
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++///


void SetCanvasStyle(TCanvas *can)
{
  gStyle->SetOptStat(0);
  gStyle->SetOptTitle(0);
  int font = 42;
  // gROOT->SetStyle("Plain");
  gStyle->SetTitleFillColor(0);
  gStyle->SetTitleBorderSize(0);
  // gStyle->SetTitleAlign(12);
  gStyle->SetLegendBorderSize(1);
  gStyle->SetTextFont(font);
  gStyle->SetTickLength(0.02, "y");
  gStyle->SetEndErrorSize(3);
  gStyle->SetLabelSize(0.04, "xyz");
  gStyle->SetLabelFont(font, "xyz");
  gStyle->SetLabelOffset(0.01, "xyz");
  gStyle->SetTitleFont(font, "xyz");
  gStyle->SetTitleOffset(1.1, "xy");
  gStyle->SetTitleSize(0.04, "xyz");
  gStyle->SetMarkerSize(1.1);
  gStyle->SetLineWidth(2);
  gStyle->SetLegendFont(42);
  gStyle->SetLegendBorderSize(0);
  gStyle->SetLegendFillColor(10);
  can->SetTickx();
  can->SetTicky();
  gStyle->SetEndErrorSize(0);
  can->SetFillColor(0);
  can->SetBorderMode(0);
  can->SetBorderSize(0);
  can->SetLeftMargin(0.18);
  can->SetRightMargin(0.1);
  can->SetBottomMargin(0.1518219);
  // can->SetTopMargin(0.);
  can->SetFrameBorderMode(0);
}
