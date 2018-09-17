#!/bin/sh
#BSUB -o test.log
eval `scram runtime -sh`
pdTreeAnalyze /lustre/cmswork/abragagn/ntuList/charmonium2018Lists/Charmonium_Run2018C-PromptReco-v1_MINIAOD_DCAP.list hist1.root -v outputFile ntu1.root -v histoMode RECREATE -v use_gen f -n 1000000 -s 1000000

