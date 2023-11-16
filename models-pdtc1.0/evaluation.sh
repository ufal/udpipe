#!/bin/bash

[ $# -ge 1 ] || { echo "Usage $0 server" >&2; exit 1; }
server="$1"

for dataset in pdt-a pdt-m pcedt-m pdtsc-m faust-m; do
  gold=../pdtc-1.0/pdtc-1.0-$dataset/pdtc-1.0-$dataset-test.conllu
  [ -f ${gold%.conllu}.paragraphs.conllu ] && gold=${gold%.conllu}.paragraphs.conllu
  for mode in raw_text gold_tok; do
    case $mode in
      raw_text) perl ../ud-2.12/conllu_to_text.pl $gold >tmp.txt; data="-F data=@tmp.txt -F tokenizer=";;
      gold_tok) data="-F data=@$gold -F input=conllu";;
    esac
    curl $data -F tagger= -F parser= $server/process | jq -j .result >tmp.conllu

    for eval in exact-match lemmaid-match; do
      case $eval in
        exact-match) process_conllu="cat";;
        lemmaid-match) process_conllu="python3 strip_lemma_info.py";;
      esac
      echo $dataset $mode $eval $(python3 ../udpipe2_eval.py --no_single_root --verbose <($process_conllu $gold) <($process_conllu tmp.conllu) | tail -n+3 | awk 'BEGIN{ORS=" "}{print $1 ":" $7}')
    done
  done
done | sh evaluation_merge_em.sh
rm tmp.txt tmp.conllu
