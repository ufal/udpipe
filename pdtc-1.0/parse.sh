#!/bin/bash

[ $# -eq 1 ] || { echo: "Usage: $0 treebank >&2"; exit 1; }
input="$1"

MODEL=models/pdtc-1.0-pdtc-1.0-a_all
qsub -q gpu-troja.q -l gpu=1 -j y -o "${input%.conllu}".predicted-conllu.log withcuda101 venv-tf2/bin/python udpipe2.py "$MODEL" --threads=4 --predict --predict_input="$input" --predict_output="${input%.conllu}".predicted-conllu
