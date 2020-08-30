#!/bin/bash

[ $# -ge 2 ] || { echo Usage: $0 datadir treebank args... >&2; exit 1; }
data="$1"; shift
treebank="$1"; shift

mkdir -p models/$data-$treebank
size=$(grep -P "^$treebank\t" $data/langs_sizes | cut -f2)
if [ "$size" -ge 400000 ]; then
  gpu_ram=16G
  args=--batch_size=64
else
  gpu_ram=8G
  args=--batch_size=32
fi
if [ "$size" -ge 50000 ]; then
  args="$args --rnn_cell_dim=512"
else
  args="$args --rnn_cell_dim=384"
fi
case $treebank in
  *_all)
    evaluation_data="--dev"
    for dev in $data/${treebank%_all}*/*-dev.conllu; do
      evaluation_data="$evaluation_data $(basename $(dirname $dev)):$dev"
    done
    evaluation_data="$evaluation_data --test"
    for test in $data/${treebank%_all}*/*-test.conllu; do
      evaluation_data="$evaluation_data $(basename $(dirname $test)):$test"
    done
    ;;
  *) evaluation_data="--dev=$data/$treebank/$treebank-ud-dev.conllu --test=$data/$treebank/$treebank-ud-test.conllu"
esac

qsub -q gpu* -l gpu=1,gpu_ram=$gpu_ram,mem_free=16G,h_data=24G -j y -o models/$data-$treebank/training.log withcuda100 venv/bin/python ud_parser.py models/$data-$treebank --train=$data/$treebank/$treebank-ud-train.conllu $evaluation_data $args "$@"
