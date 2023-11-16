#!/bin/sh

for d in pdtc-1.0-*/; do
  d=${d%/}
  for t in train dev test; do
    echo $d-$t
    python3 $(dirname $0)/compose_deprel.py --is_member --is_parenthesis_root $(for f in $(cat $d/files_$t); do echo $d/conllu/$f.conllu; done) > $d/$d-$t.conllu
  done
done
