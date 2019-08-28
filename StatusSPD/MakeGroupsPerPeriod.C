TString arrayOfPeriods[][2] = {{"2017", "LHC17h"}, {"2017", "LHC17i"}, {"2017", "LHC17k"}, {"2017", "LHC17l"}, {"2017", "LHC17m"}, {"2017", "LHC17o"}, {"2017", "LHC17r"}, {"2018", "LHC18c"}, {"2018", "LHC18d"}, {"2018", "LHC18e"}, {"2018", "LHC18f"}, {"2018", "LHC18l"}, {"2018", "LHC18m"}, {"2018", "LHC18o"}, {"2018", "LHC18p"}};
int numberOfPeriods = sizeof(arrayOfPeriods) / sizeof(arrayOfPeriods[0]);

//---------------------------------------------------------------------------------------------------//
std::vector<int> runListToVector(TString runListPath, TString splitter)
{
    std::vector<int> vectorRuns;
    TString strRunList = gSystem->GetFromPipe(Form("cat %s", runListPath.Data()));
    TObjArray *objRunList = strRunList.Tokenize(splitter);
    TIter nextRun(objRunList);
    TObjString *strRun;
    while ((strRun = (TObjString *)nextRun()))
    {
        TString currentRun = strRun->GetName();
        int runNumber = currentRun.Atoi();
        vectorRuns.push_back(runNumber);
    }
    return vectorRuns;
}
//---------------------------------------------------------------------------------------------------//

void MakeGroupsPerPeriod()
{

    std::vector<TString> vectorGroup;
    std::vector<TString> vectorRunsPerGroup;
    std::vector<int> vectorGroupSize;

    for (Int_t iPeriod = 0; iPeriod < numberOfPeriods; iPeriod++)
    {
        std::vector<int> vectorRunList = runListToVector(Form("runList_%s.txt", arrayOfPeriods[iPeriod][1].Data()), "\n");
        Int_t numberOfRunsPeriod = (int)vectorRunList.size();

        for (Int_t iRun = 0; iRun < numberOfRunsPeriod; iRun++)
        {
            TString strStatusPerRun = gSystem->GetFromPipe(Form("cat %s/%s/Log_Run_%d.txt", arrayOfPeriods[iPeriod][0].Data(), arrayOfPeriods[iPeriod][1].Data(), vectorRunList[iRun]));

            Bool_t matchIsFound = kFALSE;
            //Compare the status with the previous runs' status. If the same don't push the status string.
            for (Int_t iGroup = 0; iGroup < (int)vectorGroup.size(); iGroup++)
            {
                if (strStatusPerRun == vectorGroup[iGroup])
                {
                    matchIsFound = kTRUE;
                    vectorRunsPerGroup[iGroup].Append(Form("%s/%d\n", arrayOfPeriods[iPeriod][1].Data(), vectorRunList[iRun]));
                    vectorGroupSize[iGroup]++;
                    break;
                }
            }
            if (!matchIsFound)
            {
                vectorGroup.push_back(strStatusPerRun);
                TString strTempo;
                strTempo.Form("%s/%d\n", arrayOfPeriods[iPeriod][1].Data(), vectorRunList[iRun]);
                vectorRunsPerGroup.push_back(strTempo);
                vectorGroupSize.push_back(1);
            }
        }
    }

    cout << Form("There are %d groups of runs", (int)vectorRunsPerGroup.size()) << endl;

    ofstream outputFile_Large("LargeGroup.txt", std::ofstream::out);
    Int_t largeGroupCounter = 1;

    ofstream outputFile_Small("SmallGroup.txt", std::ofstream::out);
    Int_t smallGroupCounter = 1;

    for (Int_t iGroup = 0; iGroup < (int)vectorRunsPerGroup.size(); iGroup++)
    {

        if (vectorGroupSize[iGroup] > 5)
        {
            outputFile_Large << Form("---------------------------------------Group %d (%d runs) ---------------------------------------", largeGroupCounter, vectorGroupSize[iGroup]) << endl;
            largeGroupCounter++;
            outputFile_Large << vectorRunsPerGroup[iGroup] << endl;
        }
        else
        {
            outputFile_Small << Form("---------------------------------------Group %d (%d runs) ---------------------------------------", smallGroupCounter, vectorGroupSize[iGroup]) << endl;
            smallGroupCounter++;
            outputFile_Small << vectorRunsPerGroup[iGroup] << endl;
        }
    }

    outputFile_Large.close();
    outputFile_Small.close();

    cout<<Form("There are %d groups with more than 5 runs. The group are listed in the file LargeGroup.txt",largeGroupCounter)<<endl;
    cout<<Form("There are %d groups with more less 5 runs. The group are listed in the file SmallGroup.txt",smallGroupCounter)<<endl;
}

