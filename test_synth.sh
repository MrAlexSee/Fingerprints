#!/bin/sh

inputDir="data"
outFile="res"

dist=0
k=1
nIter=1

# Words only (no fingerprints).
for patternSize in $(seq 2 34);
do
    ./fingerprints -d -o ${outFile}.txt -D $dist -k $k --iter $nIter -f -1 --pattern-size $patternSize ${inputDir}/dict_synth.txt ${inputDir}/queries_synth.txt;
done

# All fingerprint and letters type combinations.
for fType in $(seq 0 3);
do
    for lType in $(seq 0 2);
    do
        for patternSize in $(seq 2 34);
        do
            ./fingerprints -d -o ${outFile}.txt -D $dist -k $k --iter $nIter -f $fType -l $lType --pattern-size $patternSize ${inputDir}/dict_synth.txt ${inputDir}/queries_synth.txt;
        done
    done
done
