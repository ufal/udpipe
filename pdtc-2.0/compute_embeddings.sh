#!/bin/sh

[ $# -ge 1 ] || { echo Usage: $0 data_directory embedding_args... >&2; exit 1; }
data="$1"; shift

model=robeczech-base-last4
for d in $data/pdtc-2.0*-ls/; do
  for f in $d*.conllu; do
    [ $f.npz -nt $f ] && continue
    sbatch -p gpu-ms,gpu-troja -G 1 -C "gpu_cc6.1" --mem=16G -o $f.log run withcuda118 wembedding_service/venv/bin/python wembedding_service/compute_wembeddings.py --format=conllu $f $f.npz --model=$model "$@"
  done
done
