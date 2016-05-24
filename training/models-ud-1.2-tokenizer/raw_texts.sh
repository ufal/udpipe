#!/bin/sh

data() {
  case "$2" in
    none) echo;;
    *) zcat /net/data/W2C/W2C_WEB/2011-08/$2.txt.gz | head -c 500000;;
  esac > raw_texts/$1.txt
}

nodata() {
  true
}

mkdir -p raw_texts

data ar ara
data bg bul
data cs ces
data da dan
data de deu
data el ell
data en eng
data es spa
data et est
data eu eus
data fa fas
data fi fin
data fi_ftb fin
data fr fra
data ga gle
data he heb
data hi hin
data hr hrv
data hu hun
data id ind
data it ita
data la lat
data la_itt lat
data la_proiel lat
data nl nld
data no nor
data pl pol
data pt por
data ro ron
data sl slv
data sv swe
data ta tam

# not goot matches
data grc ell
data grc_proiel ell
data cu none
data got none

nodata ja_ktc
