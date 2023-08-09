#!/bin/sh

[ $# -ge 1 ] || { echo Usage $0 datadir >&2; exit 1; }
data="$1"; shift

sh models_list.sh | while read names path treebank ack; do
  model=${names%%:*}
  for mode in "raw_text" "gold_tok"; do
    conllu=../$data/$treebank/$treebank-ud-test.conllu
    case $mode in
      raw_text) txt=${conllu%.conllu}.txt;;
      *) txt=;
    esac
    echo $model $mode $(bash ../scripts/eval_via_server.sh $model $conllu "$txt" | tail -n+3 | awk 'BEGIN{ORS=" "}{print $1 ":" $7}')
  done
done
