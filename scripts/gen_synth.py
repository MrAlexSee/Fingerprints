import numpy as np

import sys
import string

# English letter frequencies (percentages), a-z.
freq = [8.167, 1.492, 2.782, 4.253, 12.702, 2.228, 2.015, 6.094, 6.966, 0.153, 0.772, 4.025, 2.406, 6.749, 7.507, 1.929, 0.095, 5.987, 6.327, 9.056, 2.758, 0.978, 2.361, 0.150, 1.974, 0.074]
letters = [c for c in string.ascii_lowercase]

wordSize = 5 # Number of letters per word.
nWords = 100000 # Total number of words.

outFile = "out.txt" # Output file path.

def genWord(wordSize):
    pFreq = [0.01 * f for f in freq]
    curLetters = np.random.choice(letters, size = wordSize, replace = False, p = pFreq)

    return "".join(curLetters)

def genWords(nWords):
    words = []

    for i in xrange(nWords):
        perc = round(100.0 * (i + 1) / nWords)
        sys.stdout.write("\r{0}/{1} ({2}%)".format(i + 1, nWords, perc))

        words += [genWord(wordSize)]

    print ""
    return words

def main():
    print "Generating {0} words...".format(nWords)
    words = genWords(nWords)

    with open(outFile, "w") as f:
        for word in words:
            f.write(word + "\n")

    print "Dumped to: {0}, #words = {1}, word size = {2}".format(outFile, nWords, wordSize)

if __name__ == "__main__":
    main()
