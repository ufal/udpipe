#!/bin/sh

[ $# -ge 1 ] || { echo Usage $0 datadir >&2; exit 1; }
data="$1"; shift

for treebank in $(cut -f1 ../$data/langs_sizes); do
  case $treebank in *_all) continue; esac

  lang=${treebank%%_*}
  if [ -d ../$data/${lang}_all ] && ! grep -q "^$treebank\$" models_non_shared.txt; then
    rsync -av -m --delete --exclude *.v2 ../models/$data-${lang}_all/ $data-${lang}_all-200830.model
    ln -fsn $data-${lang}_all-200830.model $data-$treebank-200830.model
  else
    rsync -av -m --delete --exclude *.v2 ../models/$data-$treebank/ $data-$treebank-200830.model
  fi
done
