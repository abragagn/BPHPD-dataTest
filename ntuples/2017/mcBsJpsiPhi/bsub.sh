#!/bin/bash
i=0;
max=100;
split=1000000
while [ "$i" -le "$max" ]; do
  mkdir "s$i";
  cd "s$i";
  skip=$(python -c "print 0+$split*$i");
  echo $'#!/bin/sh' > script.sh
  echo $'#BSUB -o test.log' >> script.sh
  echo $'eval `scram runtime -sh`' >> script.sh
  echo "pdTreeAnalyze /lustre/cmswork/abragagn/ntuList/MC2017Lists/BsToJpsiPhi_RunIIFall17MiniAODv2_DCAP.list hist$i.root -v outputFile ntu$i.root -v histoMode RECREATE -v use_gen t -n $split -s $skip" >> script.sh
  echo "" >> script.sh
  bsub < script.sh;
  cd ..;
  i=`expr "$i" + 1`;
done
