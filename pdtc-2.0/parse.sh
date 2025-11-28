#!/bin/bash

[ $# -ge 2 ] || { echo "Usage: $0 input_path model [predicted_path] >&2"; exit 1; }
input="$1"; shift
model="$1"; shift
output="${1:-${input%.conllu}.predicted-conllu}"

sbatch $SLURM_ARGS -p gpu-ms,gpu-troja -G 1 -C "gpu_cc6.1" --mem=24G -o "$output".log run withcuda118 venv-tf2/bin/python udpipe2.py "$model" --threads=4 --predict=no_tagging --predict_input="$input" --predict_output="$output"
