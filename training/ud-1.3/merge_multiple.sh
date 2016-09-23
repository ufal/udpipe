#!/bin/sh

rm -rf *_all/
for lang in cs; do
  mkdir -p ${lang}_all
  for type in train dev test; do
    cat $lang*/*$type*.conllu >${lang}_all/${lang}_all-ud-$type.conllu
  done
done
