/*
Don't duplicate this macro. given a config file, it will create a run macro that can be submitted to the gri or to run locally.
*/

//-------------------------------------------------------------------------------------------------//
//Main function. It reads the config file and create a macro accordignly. The crated macro will be excuted at the end of this function.
void RunMyTask(TString configFileName = "examples/ConfigTest.txt")
{
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
    //Parameters taken from the config file
    std::vector<TString> vectorAddTaskPath;
    std::vector<TString> vectorAddTaskName;
    std::vector<TString> vectorTaskPath;
    std::vector<TString> vectorTaskName;
    std::vector<TString> vectorTaskArgument;

    TString strMacroName = "RunTask";
    TString strInputFile;
    TString strRunDir;
    TString strAliphysicsVersion = "vAN-20181106-1";
    TString strGridDataDir = "";
    TString strGridDataPattern = "";
    TString strGridRunPrefix = "";
    TString strGridWorkingDir = "";
    TString strGridOutputDir = "";
    TString strRunMode = "local";
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//

    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
    //config file reading
    TString line;
    ifstream inFile(configFileName.Data());
    if (!inFile.is_open())
    {
        cout << Form("!!!Configuration file %s cannot be opened!!!", configFileName.Data()) << endl;
        return;
    }
    while (!inFile.eof())
    {

        line.ReadLine(inFile, kTRUE);

        if (line.IsNull())
            continue;
        if(line.BeginsWith("#")){
            //This is a comment to be ignored
            continue;
        }
        TObjArray *param = line.Tokenize(" ");
        Int_t size = param->GetEntries();
        TString strTempo = ((TObjString *)param->UncheckedAt(1))->String();

        
        if (line.BeginsWith("TaskPath"))
        {
            vectorTaskPath.push_back(strTempo);
        }
        else if (line.BeginsWith("TaskName"))
        {
            vectorTaskName.push_back(strTempo);
        }
        else if (line.BeginsWith("AddTaskPath"))
        {
            vectorAddTaskPath.push_back(strTempo);
        }
        else if (line.BeginsWith("AddTaskName"))
        {
            vectorAddTaskName.push_back(strTempo);
        }
        else if (line.BeginsWith("TaskArgument"))
        {
            vectorTaskArgument.push_back(strTempo);
        }
        else if (line.BeginsWith("MacroName"))
        {
            strMacroName.Form("%s", strTempo.Data());
        }
        else if (line.BeginsWith("RunDir"))
        {
            strRunDir.Form("%s", strTempo.Data());
        }
        else if (line.BeginsWith("Input"))
        {
            strInputFile.Form("%s", strTempo.Data());
        }
        else if (line.BeginsWith("AliPhysics"))
        {
            strAliphysicsVersion.Form("%s", strTempo.Data());
        }
        else if (line.BeginsWith("GRidDataDir"))
        {
            strGridDataDir.Form("%s", strTempo.Data());
            if (strTempo.Contains("/alice/data"))
            {
                strGridRunPrefix = "000";
            }
        }
        else if (line.BeginsWith("GRidDataPattern"))
        {
            strGridDataPattern.Form("%s", strTempo.Data());
        }
        else if (line.BeginsWith("GridWorkingDir"))
        {
            strGridWorkingDir.Form("%s", strTempo.Data());
        }
        else if (line.BeginsWith("GridOutputDir"))
        {
            strGridOutputDir.Form("%s", strTempo.Data());
        }
        else if (line.BeginsWith("RunMode"))
        {
            strRunMode.Form("%s", strTempo.Data());
        }
    }
    inFile.close();
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//

    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
    //config file verifications (This will not verify the provided grid pattern and directory since they will be verified later by the alien analysis plugin)
    if ((vectorAddTaskPath.size() != vectorAddTaskName.size()) || (vectorAddTaskPath.size() != vectorTaskPath.size()) || (vectorAddTaskPath.size() != vectorTaskName.size()) || (vectorAddTaskPath.size() != vectorTaskArgument.size()))
    {
        cout << "Make sure you provide for each task, the task name and path. Same for the corresponding AddTask macro.." << endl;
        return;
    }

    if (gSystem->AccessPathName(strInputFile.Data()))
    {
        cout << Form("Can't access input file %s", strInputFile.Data()) << endl;
        return;
    }
    else
    {
        if (strRunMode == "local")
        {
            if (!strInputFile.Contains(".root"))
            {
                cout << "Only root input files are supported for local mode" << endl;
                return;
            }
        }
    }

    if (strRunMode != "runLocal")
    {
        if (strGridDataDir == "" || strGridDataPattern == "" || strGridWorkingDir == "" || strGridOutputDir == "")
        {
            cout << "Missing parameters in the config file!!" << endl;
            return;
        }
    }
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//

    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
    //copy the template adding the config file info
    TString strTemplate = gSystem->GetFromPipe(Form("cat %s/RunTaskTemplate.C", strRunDir.Data()));
    TString strTasks = "";
    TString strAddTasks = "";

    TString strAnalysisSource = "";
    TString strAdditionalLibs = "";

    for (Int_t iTask = 0; iTask < (int)vectorTaskName.size(); iTask++)
    {
        TString strClassTempo = "";
        if (!vectorAddTaskPath[iTask].Contains("$ALICE_"))
        {
            strClassTempo.Append(Form("gROOT->LoadMacro(\"%s/%s.cxx+\");\n", vectorTaskPath[iTask].Data(), vectorTaskName[iTask].Data()));
            strTasks.Append(Form("%s\n", strClassTempo.Data()));

            strAnalysisSource.Append(Form("%s.cxx ", vectorTaskName[iTask].Data()));
            strAdditionalLibs.Append(Form("%s.h %s.cxx", vectorTaskName[iTask].Data(), vectorTaskName[iTask].Data()));

            gSystem->Exec(Form("cp %s/%s* .", vectorTaskPath[iTask].Data(), vectorTaskName[iTask].Data()));
        }

        TString strAddClassTempo = "";
        strAddClassTempo.Append(Form("gROOT->LoadMacro(\"%s/%s.C\");\n", vectorAddTaskPath[iTask].Data(), vectorAddTaskName[iTask].Data()));
        strAddClassTempo.Append(Form("%s *task%d = reinterpret_cast<%s *>(gROOT->ProcessLineSync(\"%s%s\"));\n", vectorTaskName[iTask].Data(), iTask + 1, vectorTaskName[iTask].Data(), vectorAddTaskName[iTask].Data(), vectorTaskArgument[iTask].Data()));
        strAddClassTempo.Append(Form("if (!task%d) \n { \n Error(\"CreateAnalysisTrain\", \"%s not created!\"); \n return; \n }", iTask + 1, vectorAddTaskName[iTask].Data()));
        strAddTasks.Append(Form("%s\n", strAddClassTempo.Data()));
    }
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//

    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
    //Tasks section
    strTemplate.ReplaceAll("CHANGETHISTOTHECLASSLOADING", strTasks);
    strTemplate.ReplaceAll("CHANGETHISTOTHEADDCLASSLOADING", strAddTasks);
    strTemplate.ReplaceAll("CHANGETHISTOADDANALYSISSOURCE", strAnalysisSource);
    strTemplate.ReplaceAll("CHANGETHISTOADDITIONALLIBS", strAdditionalLibs);
    strTemplate.ReplaceAll("CHANGETHISTOTHEMACRONAME", strMacroName);
    strTemplate.ReplaceAll("CHANGETHISTOTASKNAME", vectorTaskName[0]);

    //Grid parameters
    strTemplate.ReplaceAll("CHANGETHISTOGRIDDATADIR", strGridDataDir);
    strTemplate.ReplaceAll("CHANGETHISTOGRIDDATAPATTERN", strGridDataPattern);
    strTemplate.ReplaceAll("CHANGETHISTORUNPREFIX", strGridRunPrefix);
    strTemplate.ReplaceAll("CHANGETHISTOGRIDWORKINGDIR", strGridWorkingDir);
    strTemplate.ReplaceAll("CHANGETHISTOGRIDOUTPUTDIR", strGridOutputDir);
    strTemplate.ReplaceAll("CHANGETHISTOALIPHYICSVERSION", strAliphysicsVersion);

    //Create the file (TODO: no need to create each time if no modification)
    ofstream outputFile(Form("%s.C", strMacroName.Data()), std::ofstream::out);
    outputFile << strTemplate << endl;
    outputFile.close();
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//

    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
    //Load and run the created macro depending on the run mode
    gROOT->LoadMacro(Form("%s.C", strMacroName.Data()));

    if (strRunMode == "local")
    {
        gROOT->ProcessLineSync(Form("RunTask(\"%s\", kRunLocal)", strInputFile.Data()));
    }
    else if (strRunMode == "test")
    {
        gROOT->ProcessLineSync(Form("RunTask(\"%s\", kRunGridTest)", strInputFile.Data()));
    }
    else if (strRunMode == "full")
    {
        gROOT->ProcessLineSync(Form("RunTask(\"%s\", kRunGridFull)", strInputFile.Data()));
    }
    else if (strRunMode == "terminate")
    {
        gROOT->ProcessLineSync(Form("RunTask(\"%s\", kRunGridTerminate)", strInputFile.Data()));
    }
    else if (strRunMode == "merge")
    {
        gROOT->ProcessLineSync(Form("RunTask(\"%s\", kRunGridMerge)", strInputFile.Data()));
    }
    else
    {
        cout << "Provided run mode is not known" << endl;
    }
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
}
//-------------------------------------------------------------------------------------------------//