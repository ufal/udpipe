#!/bin/sh

for d in pdtc-2.0-*/; do
  case "$d" in
    *-ls/) continue;;
  esac
  d=${d%/}
  mkdir -p "$d-ls"
  for f in $d/*.conllu; do
    [ -f "$d-ls/${f##*/}" ] && continue
    python3 strip_lemma_info.py $f > "$d-ls/${f##*/}"
  done
done
