#!/bin/bash

inputDir="data"
outFile="res"

dist=ham
k=1
nIter=1

# Words only (no fingerprints).
for patternSize in $(seq 2 34);
do
    ./fingerprints -d -o ${outFile}.txt -D $dist -k $k --iter $nIter -f none --pattern-size $patternSize ${inputDir}/dict_synth.txt ${inputDir}/queries_synth.txt;
done

# All fingerprint and letters type combinations.
for fType in none occ occhalved count pos;
do
    for lType in common mixed rare;
    do
        for patternSize in $(seq 2 34);
        do
            ./fingerprints -d -o ${outFile}.txt -D $dist -k $k --iter $nIter -f $fType -l $lType --pattern-size $patternSize ${inputDir}/dict_synth.txt ${inputDir}/queries_synth.txt;
        done
    done
done
