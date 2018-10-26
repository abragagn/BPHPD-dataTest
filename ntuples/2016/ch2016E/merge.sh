#!/bin/bash
i=0;
max=30;
echo $'#!/bin/sh' > park.sh
echo -n "hadd ntu2016E.root" >> park.sh
while [ "$i" -le "$max" ]; do
  echo -n " s$i/ntu$i" >> park.sh
  echo -n ".root" >> park.sh
  i=`expr "$i" + 1`;
done
echo " " >> park.sh
bash park.sh;
rm park.sh;
mv ./ntu2016E.root ../ntu2016E.root;
