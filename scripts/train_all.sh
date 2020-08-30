#!/bin/sh

[ $# -ge 2 ] || { echo Usage: $0 datadir args... >&2; exit 1; }
data="$1"; shift

for treebank in $(cut -f1 $data/langs_sizes); do
  sh train.sh $data $treebank "$@"
done
