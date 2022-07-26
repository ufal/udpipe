#!/bin/sh

[ $# -ge 1 ] || { echo Usage: $0 datadir args... >&2; exit 1; }
data="$1"; shift

for treebank in $(cut -f1 $data/langs_sizes); do
  echo Running training of $data $treebank
  sh $(dirname $0)/train.sh $data $treebank "$@"
  while [ `qstat | grep -c udpipe2` -ge `cat $(dirname $0)/train_all.limit` ]; do sleep 15; done
done
