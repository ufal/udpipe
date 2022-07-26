#!/bin/sh

[ $# -ge 1 ] || { echo Usage: $0 data_directory embedding_args... >&2; exit 1; }
data="$1"; shift

#for d in $data/*1.0-[a-z]*/; do
for d in $data/*/; do
  case $d in
    */cs_*) model=robeczech-base-last4;;
    *) model=bert-base-multilingual-uncased-last4;;
  esac

  for f in $d*.conllu; do
    [ $f.npz -nt $f ] && continue
    qsub -p 0 -q 'gpu-ms.q@!dll[691]*' -l gpu=1,mem_free=8G,h_data=16G -j y -o $f.log withcuda101 wembedding_service/venv/bin/python wembedding_service/compute_wembeddings.py --format=conllu $f $f.npz --model=$model "$@"
  done
done
