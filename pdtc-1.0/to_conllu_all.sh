#!/bin/bash

# 50 is a good number
[ $# -ge 1 ] || { echo "Usage: $0 jobs_per_treebank" >&2; exit 1; }
N=$1

for d in pdtc-1.0-*/; do
  d=${d%/}
  layer=$(echo -n $d | tail -c1)
  for i in $(seq 1 $N); do
    qsub -q cpu-*.q -o $d/conllu.log -j y bash $(dirname $0)/to_conllu.sh $d/conllu $layer $(split -n l/$i/$N $d/files_all)
  done
done
