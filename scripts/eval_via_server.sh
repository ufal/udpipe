#!/bin/bash

[ $# -ge 2 ] || { echo "Usage: $0 model conllu_data [plain_text] [server]" >&2; exit 1; }
model="$1"
data_conllu="$2"
data_txt="$3"
server="${4:-https://lindat.mff.cuni.cz/services/udpipe/api}"

python3 $(dirname $0)/../udpipe2_eval.py -v $data_conllu <(sed "${data_txt:+p;d;}"'s/\t[^\t]*$/\t_/' ${data_txt:-$data_conllu} | curl -F model=$model -F data=@- ${data_txt:+-F tokenizer=} -F tagger= -F parser= $server/process | jq -j .result)
