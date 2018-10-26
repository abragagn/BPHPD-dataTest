#!/bin/bash
i=0;
max=35;
split=1000000
while [ "$i" -le "$max" ]; do
  mkdir "s$i";
  cd "s$i";
  skip=$(python -c "print 0+$split*$i");
  echo $'#!/bin/sh' > script.sh
  echo $'#BSUB -o test.log' >> script.sh
  echo $'eval `scram runtime -sh`' >> script.sh
  echo "pdTreeAnalyze /lustre/cmswork/abragagn/ntuList/charmonium2016Lists/Charmonium_Run2016D-07Aug17-v1_AOD_DCAP.list hist$i.root -v outputFile ntu$i.root -v histoMode RECREATE -v use_gen f -n $split -s $skip" >> script.sh
  echo "" >> script.sh
  bsub < script.sh;
  cd ..;
  i=`expr "$i" + 1`;
done
