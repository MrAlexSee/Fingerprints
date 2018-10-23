#!/bin/bash

inputDir="../data"
dictName="iamerican_insane"

distanceType="ham" # ham or lev
outFile="res"

# The number of errors and iterations selected here apply only to the fingerprints executable.
k=1
nIter=1
# Dictionary subsamples of given sizes must have been generated previously.
sampleSizes=(64 256 1024 4096 16384)

# Both fingerprints and neighborhood executables should be located in the current directory.
for sampleSize in ${sampleSizes[@]$};
do
    ./neighborhood ${inputDir}/dict_${dictName}_${sampleSize}.txt ${inputDir}/queries_${dictName}_8.txt $distanceType

    # All fingerprint and letters type combinations.
    for fType in occ occhalved count pos;
    do
        for lType in common mixed rare;
        do
            ./fingerprints -d -o ${outFile}.txt -D $distanceType -k $k --iter $nIter -f $fType -l $lType ${inputDir}/dict_${dictName}_${sampleSize}.txt ${inputDir}/queries_${dictName}_8.txt;
        done
    done
done
