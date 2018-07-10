#!/bin/sh

inputDir="data"
outFile="res"
k=1

for fType in $(seq -1 3);
do
    for lType in $(seq 0 2);
    do
            ./fingerprints -d -o ${outFile}.txt -f $fType -l $lType -k $k ${inputDir}/dict_english_200.txt ${inputDir}/queries_english_200_8.txt -p1;
            ./fingerprints -d -o ${outFile}.txt -f $fType -l $lType -k $k ${inputDir}/dict_iamerican_insane.txt ${inputDir}/queries_iamerican_insane_8.txt -p1;
            ./fingerprints -d -o ${outFile}.txt -f $fType -l $lType -k $k ${inputDir}/dict_urls.txt ${inputDir}/queries_urls_69.txt -p1;
    done
done
