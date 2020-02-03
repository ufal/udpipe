#!/bin/sh

for d in */; do
  echo $d >&2
  echo ${d%/} $(grep -cP "^\d+\t" $d*train.conllu)
done | sort -rnk2 >langs_sizes

cut -d" " -f1 langs_sizes >langs
