#!/bin/bash

if [ -z "$1" ]; then
   echo "Missing filename"
   exit 127
fi

cat $1 | cut -f 3,4 -d : | sed "s/:/ /g" > $1.tmp

echo "plot '$1.tmp' with lines" | gnuplot


