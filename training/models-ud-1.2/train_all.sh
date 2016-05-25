#!/bin/bash

[ -x udpipe ] || { echo Missing udpipe >&2; exit 1; }

ls="$@"
[ -z "$ls" ] && ls=`awk '{print $1}' params_tagger`
for l in $ls; do
  mkdir -p $l
  qsub $SGE_ARGS -cwd -b y -o $l/$l.log -j y ./train.sh $l
done
