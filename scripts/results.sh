#!/bin/sh

[ $# -ge 3 ] || { echo Usage $0 treebank dev_or_test column_to_sort >&2; exit 1; }
treebank="$1"
dataset="$2"
column="$3"

{

for log in models/*$treebank${EXP:+-$EXP}/log; do
  [ -f "$log" ] || continue
  echo $(basename $(dirname $log)) $(grep -i "^$dataset  " $log | tail -n 1 | cut -d, -f3-)
done

for log in models/*${treebank%%_*}_all${EXP:+-$EXP}/log; do
  [ -f "$log" ] || continue
  echo $(basename $(dirname $log)) $(grep -i "^$dataset $treebank " $log | tail -n 1 | cut -d, -f3-)
done

} | sort -t: -rnk$column
