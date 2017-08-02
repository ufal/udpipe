#!/bin/sh

for d in ${@:-../ud-2.0/*/}; do
  l=`basename $d`
  qsub -cwd -b y -j y ./gen.sh $l
done
