#!/bin/bash

inputDir="../data"
dictName="iamerican_insane"
outFile="res"

k=1
# This applies only to the fingerprints executable.
nIter=1
# This requires that dictionary subsamples of sizes 64, 256, 1024, 4096, 16384 are generated.
sampleSizes=(64 256 1024 4096 16384)

# Both fingerprints and neighborhood executables should be located in the current directory.
for sampleSize in ${sampleSizes[@]$};
do
    ./neighborhood ${inputDir}/dict_${dictName}_${sampleSize}.txt ${inputDir}/queries_${dictName}_8.txt

    # All fingerprint and letters type combinations.
    for fType in $(seq 0 3);
    do
        for lType in $(seq 0 2);
        do
            ./fingerprints -d -o ${outFile}.txt -D 0 -k $k --iter $nIter -f $fType -l $lType ${inputDir}/dict_${dictName}_${sampleSize}.txt ${inputDir}/queries_${dictName}_8.txt;
        done
    done
done
