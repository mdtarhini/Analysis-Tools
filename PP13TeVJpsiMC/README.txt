This is a collection of macros that can be used for J/psi MC tuning in different multiplicity bins in pp at 13 TeV. There are two types of macros. The tuning one and the final comparison/plotting ones:


-------------------------------------------<TUNINIG>-------------------------------------------
-Macros: AccEffiIterator.C, FillHistoAndGetAccEffi.C, FitYields.C, GetWeights.C.
Thie main macro is "AccEffiIterator.C" which calls the ither functions for each iteration step. it can takes three arguments, the multiplicity bin, the iteration step to start with and the one to stop at. To do only one itertaion step for the first multiplicity bin (e.g step 1) run:

root -l AccEffiIterator.C\(0,1,1\)

To do multiple steps (e.g from 0 to 3), run:
root -l AccEffiIterator.C\(0, 0,3\)

It is better to the steps one by one to make sure that the fits converge for each step.


##Needed inputs:
-PtRawYields.root and RapRawYields.root: files containing the number of J/psi vs rap and pt.
-input files for the MC. The macro FillHistoAndGetAccEffi.C suports TChain reading, just make sure to properly assign the variable "inputFilesDir".
-MC-Default-Inputs: contain the rapidity and pt input shapes used originally in the MC generation.


##Output after each iteration step n > 0 and multiplicity bin x-y
-Mult-xtoy/AccEffi/iter-n/AccEffiValues.root: root file containing Acc*Eff histograms vs pt and y
-Mult-xtoy/AccEffi/iter-n/AccEffVsXX_ComparisonToIter-n-1.pdf: pdf files comparing the results to the previous step
-Mult-xtoy/RapShapeIterations/iter-n/RapYield.pdf: Rapidity yeild fitted
-Mult-xtoy/RapShapeIterations/iter-n/values.txt: rapidty fit results
-Mult-xtoy/RapShapeIterations/iter-n/RapWeight.pdf: rapidty weight (current shape divided by the previous)
-Same for pt

-------------------------------------------<Final comparison>-------------------------------------------
-Three macros can be used to produce plots after the proceudre is done for all the bins.
-CompareMultShapes.C, CompareMultYields.C, CompareFinalEffi.C