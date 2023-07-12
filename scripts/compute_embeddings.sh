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
    sbatch -p gpu-ms,gpu-troja -G 1 -C "gpu_cc6.1|gpu_cc7.5" --mem=16G -o $f.log run withcuda101 wembedding_service/venv/bin/python wembedding_service/compute_wembeddings.py --format=conllu $f $f.npz --model=$model "$@"
  done
done
