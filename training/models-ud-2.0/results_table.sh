#!/bin/sh

LEN=20
printf "|| %-${LEN}s | Mode           |  Words |  Sents |  UPOS |  XPOS |  Feats | AllTags |  Lemma |  UAS  |  LAS  |\n" Treebank

for d in */; do
  name="$(grep "^${d%/} " ../ud-2.0/iso_names.txt | awk '{print$2}' | sed 's/UD_//; s/_/ /g;')"

  has_xpos=$(grep -P "^\d+\t" ../ud-2.0/$d${d%/}-ud-test.conllu | cut -f5 | uniq | wc -l | perl -ple '$_=$_ > 1 ? 1 : 0')
  has_feats=$(grep -P "^\d+\t" ../ud-2.0/$d${d%/}-ud-test.conllu | cut -f6 | uniq | wc -l | perl -ple '$_=$_ > 1 ? 1 : 0')
  has_lemmas=$(grep -P "^\d+\t" ../ud-2.0/$d${d%/}-ud-test.conllu | cut -f3 | uniq | wc -l | perl -ple '$_=$_ > 1 ? 1 : 0')
  for type in raw goldtok goldpos; do
    case $type in
      raw)     mode="Raw text      ";;
      goldtok) mode="Gold tok      ";;
      goldpos) mode="Gold tok+morph";;
    esac

    words=$(grep "Tokenizer words" $d${d%/}.test.$type | grep -o "f1: [0-9.]*" | cut -d" " -f2 | awk '{printf "%5.1f%%%%", $0}END{if (!NR) print"   -  "}')
    sents=$(grep "Tokenizer sentences" $d${d%/}.test.$type | grep -o "f1: [0-9.]*" | cut -d" " -f2 | awk '{printf "%5.1f%%%%", $0}END{if (!NR) print"   -  "}')
    upos=$(grep "Tagging" $d${d%/}.test.$type | grep -o "upostag: [0-9.]*" | cut -d" " -f2 | awk '{printf "%4.1f%%%%", $0}END{if (!NR) print"  -  "}')
    xpos=$( ([ $has_xpos = 1 ] && grep "Tagging" $d${d%/}.test.$type | grep -o "xpostag: [0-9.]*" | cut -d" " -f2) | awk '{printf "%4.1f%%%%", $0}END{if (!NR) print"  -  "}')
    feats=$( ([ $has_feats = 1 ] && grep "Tagging" $d${d%/}.test.$type | grep -o "feats: [0-9.]*" | cut -d" " -f2) | awk '{printf "%5.1f%%%%", $0}END{if (!NR) print"   -  "}')
    alltags=$( ([ $has_xpos = 1 -a $has_feats = 1 ] && grep "Tagging" $d${d%/}.test.$type | grep -o "alltags: [0-9.]*" | cut -d" " -f2) | awk '{printf "%4.1f%%%%", $0}END{if (!NR) print"  -  "}')
    lemmas=$( ([ $has_lemmas = 1 ] && grep "Tagging" $d${d%/}.test.$type | grep -o "lemmas: [0-9.]*" | cut -d" " -f2) | awk '{printf "%5.1f%%%%", $0}END{if (!NR) print"   -  "}')
    uas=$(grep "Parsing" $d${d%/}.test.$type | grep -o "UAS: [0-9.]*" | cut -d" " -f2 | awk '{printf "%4.1f%%%%", $0}END{if (!NR) print"  -  "}')
    las=$(grep "Parsing" $d${d%/}.test.$type | grep -o "LAS: [0-9.]*" | cut -d" " -f2 | awk '{printf "%4.1f%%%%", $0}END{if (!NR) print"  -  "}')

    printf " | %-${LEN}s | $mode | $words | $sents | $upos | $xpos | $feats |   $alltags | $lemmas | $uas | $las |\n" "$name"
  done
done | sort -sk2,2
