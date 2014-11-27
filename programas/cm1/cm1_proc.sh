#!/bin/bash

CSIZE_LIST=`cat cm1.out | cut -f1 | cut -d"=" -f2 |sort -gu`
STRIDE_LIST=`cat cm1.out | cut -f2 | cut -d"=" -f2 |sort -gu`

rm cm1_plot.dat 2> /dev/null
touch cm1_plot.dat

for stride in $STRIDE_LIST; do
  VALUE=""
  for cache in $CSIZE_LIST; do
    METRIC=`cat cm1.out | grep "CSIZE=$cache " | grep "STRIDE=$stride " | cut -f3 | cut -d"=" -f2`
    if [[ -z $METRIC ]]; then 
      METRIC=0.0; 
    fi
    VALUE="$VALUE $METRIC"
  done
  echo "$stride $VALUE" >> cm1_plot.dat
done

echo "call \"cm1_plot.gp\"" | gnuplot

