#!/bin/bash

for d in ../ud-2.6/*/; do
  d=$(basename ${d%/})
  lines=$(diff <(awk -F "[\t]" '/# new/{print} /^[0-9]*\t/{print $1,$2,(($10 ~ /SpaceAfter=No/) ? "SA=No" : "")}' ../ud-2.6/$d/*.conllu) <(awk -F "[\t]" '/# new/{print} /^[0-9]*\t/{print $1,$2,(($10 ~ /SpaceAfter=No/) ? "SA=No" : "")}' ../ud-2.5/$d/*.conllu) | wc -l)
  [ "$lines" -eq 0 ] && { echo -n Copying "" >&2; grep "^$d " ../models-ud-2.5-tokenizer-chosen/params_tokenizer; }
  echo $d $lines >&2
done
