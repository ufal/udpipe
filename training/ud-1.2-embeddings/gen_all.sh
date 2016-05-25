#!/bin/sh

for d in ../../ud-1.2/*/; do
  l=`basename $d`
  qsub -cwd -b y ./gen.sh $l
done
