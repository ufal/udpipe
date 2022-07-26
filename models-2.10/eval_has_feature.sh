#!/bin/sh

for d in ../ud-2.10/*/; do
  case $d in
    *_all/) continue;;
  esac

  has_xpos=$(grep -P "^\d+\t" $d$(basename ${d%/})-ud-test.conllu | cut -f5 | uniq | wc -l | perl -ple '$_=$_ > 1 ? 1 : 0')
  has_feats=$(grep -P "^\d+\t" $d$(basename ${d%/})-ud-test.conllu | cut -f6 | uniq | wc -l | perl -ple '$_=$_ > 1 ? 1 : 0')
  has_lemmas=$(grep -P "^\d+\t" $d$(basename ${d%/})-ud-test.conllu | cut -f3 | uniq | wc -l | perl -ple '$_=$_ > 1 ? 1 : 0')
  [ $has_xpos = 0 ] && echo $d: no XPOS
  [ $has_feats = 0 ] && echo $d: no UFeats
  [ $has_lemmas = 0 ] && echo $d: no Lemmas
done
