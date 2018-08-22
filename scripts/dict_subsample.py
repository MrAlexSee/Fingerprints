"""
Extracts randomly selected dictionary subsamples of given sizes.
"""

import os
import random

# Sizes of extracted samples.
pSampleSizes = [64, 256, 1024, 4096, 16384]

# All samples should include only words with pWordSize letter count. Set to -1 to ignore.
pWordSize = 8

# Input file path.
pInFile = "../data/dict_iamerican_insane.txt"
# Output (base) file path.
pOutFile = "sample.txt"

def readWords(inFile):
    print "Reading from: {0}".format(inFile)

    with open(inFile, "r") as f:
        words = f.read()

    words = words.split("\n")
    print "Read #words = {0}".format(len(words))

    return words

def doSampleWithSize(words, sampleSize, wordSize):
    curWords = [word for word in words if len(word) == wordSize]
    print "Sampling {0} from #words = {1} (size = {2})".format(sampleSize, len(curWords), wordSize)

    sample = random.sample(curWords, sampleSize)

    assert len(sample) == sampleSize
    return sample

def doSampleAll(words, sampleSize):
    return random.sample(words, sampleSize)

def main():
    words = readWords(pInFile)

    for sampleSize in pSampleSizes:
        if pWordSize > 0:
            sample = doSampleWithSize(words, sampleSize, pWordSize)
        else:
            sample = doSampleAll(words, sampleSize)

        tup = os.path.splitext(pOutFile)
        outFileName = "{0}_{1}{2}".format(tup[0], len(sample), tup[1])

        with open(outFileName, "w") as f:
            f.write("\n".join(sample))
        
        print "Dumped #words = {0} to: {1}".format(len(sample), outFileName)

if __name__ == "__main__":
    main()
