#!/bin/sh

data() {
  case "$2" in
    none) echo;;
    *) zcat /net/data/W2C/W2C_WEB/2011-08/$2.txt.gz | head -c 500000;;
  esac > $1.txt
}

data de_hdt deu
data fi_ftb fin
data fr_spoken fra
data fro_srcmf fra
data ko_kaist kor
data la_proiel lat
data no_bokmaal nor
data no_nynorsklia nno
data sl_sst slv
data te_mtg tel
