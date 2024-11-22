#!/bin/sh

[ $# -ge 1 ] || { echo Usage $0 dataset dev_or_test >&2; exit 1; }
data="$1"; shift

prefix=models/$data
for d in $prefix-*_all; do
  for t in $d/*/; do
    t=$(basename $t)
    echo $t
    for s in $@; do
      grep -i "$s.*epoch 20, lr 0.0001" $prefix-$t/training.log
      grep -i "$s.*$t epoch 20, lr 0.0001" $d/training.log
    done
  done
done | python3 $(dirname $0)/results_shared_compare_summary.py
