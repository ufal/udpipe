#!/bin/bash

lang="$1"; shift

mkdir -p $lang
./udpipe --train --tokenizer=`grep "^$lang " params_tokenizer | sed "s/^$lang //"` --tagger=`grep "^$lang " params_tagger | sed "s/^$lang //"` --parser=`grep "^$lang " params_parser | sed "s/^$lang //"` --heldout=../ud-2.0/$lang/$lang-ud-dev.conllu $lang/$lang.model ../ud-2.0/$lang/$lang-ud-train.conllu

for type in dev test; do
  ./udpipe --accuracy --tokenize --tag --parse "$lang/$lang.model" <../ud-2.0/$lang/$lang-ud-$type.conllu >"$lang/$lang.$type.raw"
  ./udpipe --accuracy --tag --parse $lang/$lang.model <../ud-2.0/$lang/$lang-ud-$type.conllu >$lang/$lang.$type.goldtok
  ./udpipe --accuracy --parse $lang/$lang.model <../ud-2.0/$lang/$lang-ud-$type.conllu >$lang/$lang.$type.goldpos

#  python conll17_ud_eval.py -v ../ud-2.0/$lang/$lang-ud-$type.conllu <(./udpipe --tokenize --tag --parse $lang/$lang.model ../ud-2.0/$lang/$lang-ud-$type.txt) >$lang/$lang.c17-$type
done
