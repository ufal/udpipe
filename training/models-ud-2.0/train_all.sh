#!/bin/bash

[ -x udpipe ] || { echo Missing udpipe >&2; exit 1; }

ls="$@"
[ -z "$ls" ] && ls=`awk '{print $1}' params_parser`
for l in $ls; do
  mkdir -p $l
  qsub -q troja*@* $SGE_ARGS -cwd -b y -o $l/$l.log -j y ./train.sh $l
done
