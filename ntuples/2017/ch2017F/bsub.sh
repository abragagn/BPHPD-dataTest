#!/bin/bash
i=0;
max=75;
split=1000000
while [ "$i" -le "$max" ]; do
  mkdir "s$i";
  cd "s$i";
  skip=$(python -c "print 0+$split*$i");
  echo $'#!/bin/sh' > script.sh
  echo $'#BSUB -o test.log' >> script.sh
  echo $'eval `scram runtime -sh`' >> script.sh
  echo "pdTreeAnalyze /lustre/cmswork/abragagn/ntuList/charmonium2017Lists/Charmonium_Run2017F-31Mar2018-v1_MINIAOD_DCAP.list hist$i.root -v outputFile ntu$i.root -v histoMode RECREATE -v use_gen f -n $split -s $skip" >> script.sh
  echo "" >> script.sh
  bsub < script.sh;
  cd ..;
  i=`expr "$i" + 1`;
done
