outFile="res.txt"

echo "1/4 Rebuilding"
cd ..
make rebuild

echo "2/4 Copying"
cp fingerprints end_to_end_tests/fingerprints
cd end_to_end_tests

echo "3/4 Running"

# Hamming distance. All fingerprint and letters type combinations.
for fType in none occ occhalved count pos;
do
    for lType in common mixed rare;
    do
        ./fingerprints -D ham -k 1 -f $fType -l $lType dict_test.txt queries_test.txt > $outFile
        python check_result.py 3
    done
done

# Levenshtein distance. All fingerprint and letters type combinations.
for fType in none occ occhalved count pos;
do
    for lType in common mixed rare;
    do
        ./fingerprints -D lev -k 1 -f $fType -l $lType dict_test.txt queries_test.txt > $outFile
        python check_result.py 4
    done
done

echo "4/4 Teardown"
rm -f fingerprints $outFile
