"""
Generates a synthetic dictionary based on English alphabet letter frequencies.
"""

import sys
import string

import numpy as np

# English letter frequencies (percentages). First line: [a-j], second line: [k-t], third line: [u-z].
pFreqList = [8.167, 1.492, 2.782, 4.253, 12.702, 2.228, 2.015, 6.094, 6.966, 0.153,
             0.772, 4.025, 2.406, 6.749, 7.507, 1.929, 0.095, 5.987, 6.327, 9.056, 
             2.758, 0.978, 2.361, 0.150, 1.974, 0.074]
pLetters = [c for c in string.ascii_lowercase]

# Number of letters per word.
pWordSize = 5
# Total number of words.
pNWords = 1000

# Output file path.
pOutFile = "dict.txt"

def genWord(wordSize, letters, freqList):
    pFreq = [0.01 * f for f in freqList]
    curLetters = np.random.choice(letters, size = wordSize, replace = False, p = pFreq)

    return "".join(curLetters)

def genWords(nWords, wordSize, letters, freqList):
    words = []

    for i in xrange(nWords):
        perc = round(100.0 * (i + 1) / nWords)
        sys.stdout.write("\r{0}/{1} ({2}%)".format(i + 1, nWords, perc))

        words += [genWord(wordSize, letters, freqList)]

    print ""
    return words

def main():
    print "Generating {0} words...".format(pNWords)
    words = genWords(pNWords, pWordSize, pLetters, pFreqList)

    with open(pOutFile, "w") as f:
        f.write("\n".join(words))

    assert len(words) == pNWords
    print "Dumped to: {0}, #words = {1}, word size = {2}".format(pOutFile, pNWords, pWordSize)

if __name__ == "__main__":
    main()
