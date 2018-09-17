#!/bin/bash
i=0;
max=20;
echo $'#!/bin/sh' > park.sh
echo -n "hadd ntu2018C.root" >> park.sh
while [ "$i" -le "$max" ]; do
  echo -n " s$i/ntu$i" >> park.sh
  echo -n ".root" >> park.sh
  i=`expr "$i" + 1`;
done
echo " " >> park.sh
bash park.sh;
rm park.sh;
mv ./ntu2018C.root ../ntu2018C.root;
