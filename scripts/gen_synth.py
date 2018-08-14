"""
Generates a synthetic dictionary based on English alphabet letter frequencies.
"""

import os
import sys
import string

import numpy as np

# English letter frequencies (percentages) for sampling. First line: [a-j], second line: [k-t], third line: [u-z].
pFreqList = [8.167, 1.492, 2.782, 4.253, 12.702, 2.228, 2.015, 6.094, 6.966, 0.153,
             0.772, 4.025, 2.406, 6.749, 7.507, 1.929, 0.095, 5.987, 6.327, 9.056, 
             2.758, 0.978, 2.361, 0.150, 1.974, 0.074]
# English letters for sampling.
pLetters = [c for c in string.ascii_lowercase]

# If true, generates words of size pWordSize, otherwise generates words with sizes from pWordSizeRange.
pSingleSize = False
# Number of letters per word.
pWordSize = 5
# Range of number of letters per word.
pWordSizeRange = range(2, 34 + 1, 1)

# Total number of words.
pNWords = 500000

# Output (base) file path.
pOutFile = "dict.txt"

def genWord(wordSize, letters, freqList):
    fracFreqList = [0.01 * f for f in freqList]
    
    assert len(letters) == len(fracFreqList)
    curLetters = np.random.choice(letters, size = wordSize, replace = True, p = fracFreqList)

    return "".join(curLetters)

def genWords(nWords, wordSize, letters, freqList):
    words = []

    for i in xrange(nWords):
        perc = round(100.0 * (i + 1) / nWords)
        sys.stdout.write("\r{0}: {1}/{2} ({3}%)".format(wordSize, i + 1, nWords, perc))

        words += [genWord(wordSize, letters, freqList)]

    print ""
    return words

def main():
    print "Generating {0} words...".format(pNWords)
    tup = os.path.splitext(pOutFile)

    if pSingleSize:
        print "Single word size = {0}".format(pWordSize)
        outFileName = "{0}{1}{2}".format(tup[0], pWordSize, tup[1])

        words = genWords(pNWords, pWordSize, pLetters, pFreqList)
    else:
        print "Word sizes = [{0}, {1}]".format(pWordSizeRange[0], pWordSizeRange[-1])
        outFileName = "{0}{1}-{2}{3}".format(tup[0], pWordSizeRange[0], pWordSizeRange[-1], tup[1])

        words = []
        nWords = int(pNWords / len(pWordSizeRange))

        for curWordSize in pWordSizeRange:
            words += genWords(nWords, curWordSize, pLetters, pFreqList)

    with open(outFileName, "w") as f:
        f.write("\n".join(words))

    totalSizeMB = float(sum([len(word) for word in words])) / 1000000.0
    print "Dumped to: {0}, total #words = {1}, total size = {2:.2f} MB".format(outFileName, len(words), totalSizeMB)

if __name__ == "__main__":
    main()
