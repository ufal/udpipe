#!/bin/bash

set -e

for model in models/pdtc-2.0-*; do
  model=$(basename $model)
  for mode in raw dict; do
    case $mode in
      raw) predict="--predict=no_morphodita" ;;
      dict) predict="--predict" ;;
    esac
    for tb in a faust-a pcedt-a pdt-a pdt-m pdtsc-a; do
      src_ls=pdtc-2.0/pdtc-2.0-$tb-ls/pdtc-2.0-$tb-test.conllu
      tgt=pdtc-2.0/results/pdtc-2.0-$tb.${model#pdtc-2.0-pdtc-2.0-}.$mode.conllu

      withcuda118 venv-tf2/bin/python3 udpipe2.py models/$model $predict --predict_input=$src_ls --predict_output=$tgt

      case $mode in
        dict) venv-tf2/bin/python3 udpipe2_eval.py --no_single_root -v ${src_ls/-ls\//\/} $tgt >$tgt.eval
      esac
      venv-tf2/bin/python3 udpipe2_eval.py --no_single_root -v $src_ls <(venv-tf2/bin/python3 pdtc-2.0/strip_lemma_info.py $tgt) >$tgt-ls.eval
    done
  done
done

echo All done
