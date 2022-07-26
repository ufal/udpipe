#!/bin/sh

[ $# -ge 2 ] || { echo Usage $0 datadir date >&2; exit 1; }
data="$1"; shift
date="$1"; shift

for treebank in $(cut -f1 ../$data/langs_sizes); do
  case $treebank in *_all) continue; esac

  lang=${treebank%%_*}
  if [ -d ../$data/${lang}_all ] && ! grep -q "^$treebank\$" models_non_shared.txt; then
    rsync -av -m --delete --exclude *.v2 --exclude training.log ../models/$data-${lang}_all/ ${lang}_all-$data-$date.model
    ln -fsn ${lang}_all-$data-$date.model $treebank-$data-$date.model
  else
    rsync -av -m --delete --exclude *.v2 --exclude training.log ../models/$data-$treebank/ $treebank-$data-$date.model
  fi
done

for treebank in $(cut -f1 ../$data/langs_sizes); do
  case $treebank in *_all) continue; esac
  cp ~/repos/udpipe/training/models-ud-2.10-tokenizer-chosen/$treebank/$treebank.model $treebank-$data-$date.model/$treebank.tokenizer
done
