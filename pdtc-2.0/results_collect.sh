#!/bin/sh

for model in models/pdtc-2.0-*; do
  model=$(basename $model)
  echo "$model - RawTags | RawLemmas | Tags | Lemmas | LemmasEM | UAS | LAS"
  for tb in a faust-a pcedt-a pdt-a pdtsc-a; do
    printf %-7s $tb
    for item in raw:-ls:XPOS raw:-ls:Lemmas dict:-ls:XPOS dict:-ls:Lemmas dict::Lemmas dict::UAS dict::LAS; do
      mode=${item%%:*}
      item=${item#*:}
      ls_mode=${item%%:*}
      metric=${item#*:}
      echo -n " & $(grep "^$metric" pdtc-2.0/results/pdtc-2.0-$tb.${model#pdtc-2.0-pdtc-2.0-}.$mode.conllu$ls_mode.eval | awk '{print $NF}')"
    done
    echo " \\\\\\"
  done | awk -F"&" '
    {print}
    NR>1 {sum1+=$2; sum2+=$3; sum3+=$4; sum4+=$5; sum5+=$6; sum6+=$7; sum7+=$8; c++ }
    END {printf "macro_a & %.2f & %.2f & %.2f & %.2f & %.2f & %.2f & %.2f \\\\\n", sum1/c, sum2/c, sum3/c, sum4/c, sum5/c, sum6/c, sum7/c}'
done
