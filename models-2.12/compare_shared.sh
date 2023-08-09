#!/bin/sh

[ $# -ge 1 ] || { echo Usage $0 prefix >&2; exit 1; }
prefix="$1"; shift

for d in $prefix-*_all; do
  for t in $d/*/; do
    t=$(basename $t)
    echo $t
    for s in Dev Test; do
      grep "$s.*epoch 20, lr 0.0001" $prefix-$t/training.log
      grep "$s.*$t epoch 20, lr 0.0001" $d/training.log
    done
  done
done
