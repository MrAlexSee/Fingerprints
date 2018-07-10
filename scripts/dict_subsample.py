"""
Extracts randomly selected dictionary subsamples of given sizes.
"""

import os
import random

# Sizes of extracted samples.
pSampleSizes = [64, 512, 4096, 32768, 262144]

# Input file path.
pInFile = "../data/dict_iamerican_insane.txt"
# Output (base) file path.
pOutFile = "sample.txt"

def readWords(inFile):
    print "Reading from: {0}".format(inFile)

    with open(inFile, "r") as f:
        words = f.read()

    words = words.split()
    print "Read #words = {0}".format(len(words))

    return words

def main():
    words = readWords(pInFile)

    for sampleSize in pSampleSizes:
        sample = random.sample(words, sampleSize)

        tup = os.path.splitext(pOutFile)
        outFileName = "{0}{1}{2}".format(tup[0], len(sample), tup[1])

        with open(outFileName, "w") as f:
            f.write("\n".join(sample))
        
        print "Dumped #words = {0} to: {1}".format(len(sample), outFileName)

if __name__ == "__main__":
    main()
