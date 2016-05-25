#!/bin/bash

for d in ${@:-*/}; do
  l=`basename $d`

  echo $l
  [ -f "$l/$l.test" ] && sed 's/^/  /' $l/$l.test
  echo
done
