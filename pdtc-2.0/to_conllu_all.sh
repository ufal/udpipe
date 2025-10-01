#!/bin/bash

# 50 is a good number
[ $# -ge 1 ] || { echo "Usage: $0 jobs_per_treebank" >&2; exit 1; }
N=$1

for d in pdtc-2.0-*/; do
  d=${d%/}
  layer=$(echo -n $d | tail -c1)
  for i in $(seq 1 $N); do
    sbatch -p cpu-troja -o $d/conllu-$i.log run bash $(dirname $0)/to_conllu.sh $d/conllu $layer $(split -n l/$i/$N $d/files_all)
  done
done
