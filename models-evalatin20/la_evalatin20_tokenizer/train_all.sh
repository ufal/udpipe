#!/bin/bash

[ -x udpipe ] || { echo Missing udpipe >&2; exit 1; }

index=1
while [ -f train_all_$index.cmds ]; do
  index=`expr $index + 1`
done
cmd_file=train_all_$index.cmds

>$cmd_file
for d in ${@:-la_evalatin20}; do
  lang=`basename "$d"`
  for args in dimension={24,64}\;epochs=100\;segment_size={50,200}\;initialization_range={0.2,0.1}\;batch_size=50\;learning_rate={0.01,0.005,0.002}\;learning_rate_final={0,0.001}\;dropout={0.1,0.2,0.3}\;early_stopping=1; do
    id="${args// /_}"
    id="${id//\//_}"
    id="${id//,/-}"
    id="${id//;/-}"
    id="${id//:/}"
    id="${id//dimension/dim}"
    id="${id//batch_size/bs}"
    id="${id//learning_rate_final/lf}"
    id="${id//learning_rate/l}"
    id="${id//dropout/d}"

    echo ./train.sh $lang "$id" "$args" >>$cmd_file
  done
done

tasks=$(wc -l <$cmd_file)
echo qsub -q ${Q:-cpu-troja.q@*} $SGE_ARGS -cwd -b y -o $cmd_file.log -j y -t 1-$tasks -tc ${SGE_CONCURRENT:-400} arrayjob_runner $cmd_file
