#!/bin/sh

for d in */; do
  echo $d >&2
  echo ${d%/} $(grep -cP "^\d+\t" $d*train.conllu) $(grep "^License:" $d/README.*)
done
