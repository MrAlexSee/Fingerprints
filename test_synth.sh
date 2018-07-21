#!/bin/sh

inputDir="data"
outFile="res"

dist=0
k=1

for fType in $(seq -1 3);
do
    for lType in $(seq 0 2);
    do
        for patternSize in $(seq 1 32);
        do
            ./fingerprints -d -o ${outFile}.txt -f $fType -l $lType -k $k -D $dist --pattern-size $patternSize ${inputDir}/dict_synth.txt ${inputDir}/queries_synth.txt;
        done
    done
done
