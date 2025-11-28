#!/bin/bash

[ $# -ge 2 ] || { echo Usage: $0 datadir treebank args... >&2; exit 1; }
data="$1"; shift
treebank="$1"; shift

mkdir -p models/$data-$treebank${EXP:+-$EXP}
args=
args="$args --batch_size=64"
args="$args --rnn_cell_dim=512"
args="$args --wembedding_model=robeczech-base-last4"
args="$args --single_root=0"
args="$args --morphodita=czech-morfflex2.1-250909.dict"

for ds in dev test; do
  args="$args --$ds"
  for tb in "a" "faust-a" "pcedt-a" "pdt-a" "pdt-m" "pdtsc-a"; do
    args="$args pdtc-2.0-$tb-ls:$data/pdtc-2.0-$tb-ls/pdtc-2.0-$tb-$ds.conllu"
  done
done

sbatch $SLURM_ARGS -p gpu-ms,gpu-troja -G 1 -C "gpu_cc6.1" --mem=24G -o models/$data-$treebank${EXP:+-$EXP}/training.log run withcuda100 venv/bin/python udpipe2.py models/$data-$treebank${EXP:+-$EXP} --train $data/$treebank/*-train.conllu $evaluation_data $args "$@"
