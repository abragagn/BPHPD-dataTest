#!/bin/sh
#BSUB -o test.log
eval `scram runtime -sh`
pdTreeAnalyze /lustre/cmswork/abragagn/ntuList/charmonium2018Lists/Charmonium_Run2018A-PromptReco-v2_MINIAOD_DCAP.list hist14.root -v outputFile ntu14.root -v histoMode RECREATE -v use_gen f -n 1000000 -s 14000000

