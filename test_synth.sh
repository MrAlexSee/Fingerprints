#!/bin/sh

inputDir="data"
outFile="res"

dist=0
k=1
nIter=100

for fType in $(seq -1 3);
do
    for lType in $(seq 0 2);
    do
        for patternSize in $(seq 2 34);
        do
            ./fingerprints -d -o ${outFile}.txt -D $dist -k $k --iter $nIter -f $fType -l $lType --pattern-size $patternSize ${inputDir}/dict_synth.txt ${inputDir}/queries_synth.txt;
        done
    done
done
