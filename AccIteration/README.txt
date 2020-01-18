Thie main macro is "AccEffiIterator.C" which calls the ither functions for each iteration step. it can takes two arguments, the iteration step to start with and the one to stop at. To do only one itertaion step (e.g step 1) run:

root -l AccEffiIterator.C\(1,1\)

To do multiple steps (e.g from 0 to 3), run:
root -l AccEffiIterator.C\(0,3\)

It is better to the steps one by one to make sure that the fits converge for each step.


##Needed inputs:
-NPsi2s.root: a file containing the number of pis(2s) vs rap and pt. Use the macro FillNpsi2s.C to fill it.
-GridAnalysis/AnalysisResults.root: Contains the MC tree after analysing the AODs
-RapShapeIterations/iter-0/values.txt and  PtShapeIterations/iter-0/values.txt : contain the rapidity and pt input shapes used in the MC generation.


##Output after each iteration step n > 0
-AccEffi/iter-n/AccEffiValues.root: root file containing Acc*Eff histograms vs pt and y
-AccEffi/iter-n/AccEffVsXX_ComparisonToIter-n-1.pdf: pdf files comparing the results to the previous step
-RapShapeIterations/iter-n/RapYield.pdf: Rapidity yeild fitted
-RapShapeIterations/iter-n/values.txt: rapidty fit results
-RapShapeIterations/iter-n/RapWeight.pdf: rapidty weight (current shape divided by the previous)
-Same for pt