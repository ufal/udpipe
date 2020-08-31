#!/bin/sh

[ $# -ge 2 ] || { echo Usage: $0 data_directory embedding_args... >&2; exit 1; }
data="$1"; shift

for d in $data/*/; do
  for f in $d*.conllu; do
    qsub -p 0 -q gpu* -l gpu=1,mem_free=8G,h_data=16G -j y -o $f.log withcuda101 wembedding_service/venv/bin/python wembedding_service/compute_wembeddings.py --format=conllu $f $f.npz "$@"
  done
done
