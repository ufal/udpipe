#!/bin/sh

[ $# -ge 1 ] || { echo Usage $0 modeldir >&2; exit 1; }
modeldir="$1"; shift

for d in $modeldir/*/; do
  [ -L ${d%/} ] && continue
  for f in $d/*.tokenizer; do
    echo $(basename $f .tokenizer) $(grep -e '^Test  ' -e "^Test $(basename $f .tokenizer) " $d/log | grep -o 'UPOS.*$')
  done
done | column -nt
