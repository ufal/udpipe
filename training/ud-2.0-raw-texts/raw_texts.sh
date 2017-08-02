#!/bin/sh

data() {
  case "$2" in
    none) echo;;
    *) zcat /net/data/W2C/W2C_WEB/2011-08/$2.txt.gz | head -c 500000;;
  esac > $1.txt
}

nodata() {
  true
}

data da dan
data fi_ftb fin
data la_proiel lat
data sl_sst slv
