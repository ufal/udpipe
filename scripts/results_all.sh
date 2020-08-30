#!/bin/sh

[ $# -ge 1 ] || { echo Usage $0 datadir dev_or_test >&2; exit 1; }
data="$1"; shift
dataset="$1"; shift

for lang in ${@:-$data/*_all/}; do
  lang=$(basename $lang _all)
  for treebank in $data/${lang}_*/; do
    treebank=$(basename $treebank)
    [ "${treebank##*_}" = all ] && continue
    echo $treebank $(sh $(dirname $0)/results.sh $treebank test 1 | sed 's/_all/-all/' | sort -r | $(dirname $0)/results_diff.py | tail -n 1)
  done
done
