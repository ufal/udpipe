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
    for dev in $data/${treebank%_all}*/*-dev.conllu; do
      evaluation_data="$evaluation_data $(basename $(dirname $dev)):$dev"
    done
    evaluation_data="$evaluation_data --test"
    for test in $data/${treebank%_all}*/*-test.conllu; do
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

qsub $SGE_ARGS -q gpu* -l gpu=1,gpu_ram=11G,mem_free=16G,h_data=24G -N udpipe2 -j y -o models/$data-$treebank${EXP:+-$EXP}/training.log withcuda100 venv/bin/python ud_parser16.py models/$data-$treebank${EXP:+-$EXP} --train $data/$treebank/$treebank-*train.conllu $evaluation_data $args "$@"
