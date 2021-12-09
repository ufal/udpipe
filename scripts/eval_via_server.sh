#!/bin/bash

[ $# -ge 2 ] || { echo "Usage: $0 model conllu_data [plain_text]" >&2; exit 1; }
model="$1"
data_conllu="$2"
data_txt="$3"

python3 $(dirname $0)/../udpipe2_eval.py -v $data_conllu <(curl -F model=$model -F data=@${data_txt:-$data_conllu} ${data_txt:+-F tokenizer=} -F tagger= -F parser= https://lindat.mff.cuni.cz/services/udpipe/api/process | jq -j .result)
