#!/bin/bash

[ $# -ge 2 ] || { echo Usage: $0 datadir treebank args... >&2; exit 1; }
data="$1"; shift
treebank="$1"; shift

mkdir -p models/$data-$treebank${EXP:+-$EXP}
size=$(grep -P "^$treebank\t" $data/langs_sizes | cut -f2)
if [ "$size" -ge 400000 ]; then
  args=--batch_size=64
else
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
    for dev in $data/${treebank%_all}_*/*-dev.conllu; do
      evaluation_data="$evaluation_data $(basename $(dirname $dev)):$dev"
    done
    evaluation_data="$evaluation_data --test"
    for test in $data/${treebank%_all}_*/*-test.conllu; do
      evaluation_data="$evaluation_data $(basename $(dirname $test)):$test"
    done
    ;;
  la_evalatin20)
    args="$args --tags=UPOS,LEMMAS --parse=0 --rnn_layers=3"
    evaluation_data="--dev=$data/la_evalatin20/la_evalatin20-dev.conllu --test"
    for kind in "" "-crossgenre" "-crosstime"; do
      evaluation_data="$evaluation_data ${kind:+la_evalatin20$kind}:$data/la_evalatin20/la_evalatin20-test$kind.conllu"
    done
    ;;
  *) evaluation_data="--dev=$data/$treebank/$treebank-ud-dev.conllu --test=$data/$treebank/$treebank-ud-test.conllu"
esac

sbatch $SLURM_ARGS -p gpu-ms,gpu-troja -G 1 -C "gpu_cc6.1|gpu_cc7.5" --mem=24G -o models/$data-$treebank${EXP:+-$EXP}/training.log run withcuda100 venv/bin/python udpipe2.py models/$data-$treebank${EXP:+-$EXP} --train $data/$treebank/$treebank-*train.conllu $evaluation_data $args "$@"
