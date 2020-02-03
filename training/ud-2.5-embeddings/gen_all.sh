#!/bin/bash

for d in ${@:-../ud-2.5/*/}; do
  l=`basename $d`
  qsub -q cpu-troja.q -cwd -b y -j y ./gen.sh $l
done
