outFile="res.txt"

echo "1/4 Rebuilding"
cd ..
make rebuild

echo "2/4 Copying"
cp fingerprints end_to_end_tests/fingerprints
cd end_to_end_tests

echo "3/4 Running"

# All fingerprint and letters type combinations.
for fType in $(seq -1 3);
do
    for lType in $(seq 0 2);
    do
        ./fingerprints -D 0 -k 1 -f $fType -l $lType dict_test.txt queries_test.txt > $outFile
        python check_result.py
    done
done

echo "4/4 Teardown"
rm -f fingerprints $outFile
