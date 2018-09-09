#!/bin/bash

inputDir="data"
outFile="res"

dist=ham
k=1
nIter=1

# Words only (no fingerprints).
./fingerprints -d -o ${outFile}.txt -D $dist -k $k --iter $nIter -f none ${inputDir}/dict_iamerican_insane.txt ${inputDir}/queries_iamerican_insane_8.txt;
./fingerprints -d -o ${outFile}.txt -D $dist -k $k --iter $nIter -f none ${inputDir}/dict_english_200.txt ${inputDir}/queries_english_200_8.txt;
./fingerprints -d -o ${outFile}.txt -D $dist -k $k --iter $nIter -f none ${inputDir}/dict_urls.txt ${inputDir}/queries_urls_69.txt;

# All fingerprint and letters type combinations.
for fType in occ occhalved count pos;
do
    for lType in common mixed rare;
    do
        ./fingerprints -d -o ${outFile}.txt -D $dist -k $k --iter $nIter -f $fType -l $lType ${inputDir}/dict_iamerican_insane.txt ${inputDir}/queries_iamerican_insane_8.txt;
        ./fingerprints -d -o ${outFile}.txt -D $dist -k $k --iter $nIter -f $fType -l $lType ${inputDir}/dict_english_200.txt ${inputDir}/queries_english_200_8.txt;
        ./fingerprints -d -o ${outFile}.txt -D $dist -k $k --iter $nIter -f $fType -l $lType ${inputDir}/dict_urls.txt ${inputDir}/queries_urls_69.txt;
    done
done
