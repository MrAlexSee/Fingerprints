"""
Prints a histogram of word lengths and mode length for a dictionary.
"""

# Input file path.
pInFile = "../data/dict_urls.txt"

def readWords(inFile):
    print "Reading from: {0}".format(inFile)

    with open(inFile, "r") as f:
        words = f.read()

    words = words.split("\n")
    words = [w for w in words if w]

    print "Read #words = {0}".format(len(words))
    return words

def main():
    words = readWords(pInFile)

    hist = {}
    for w in words:
        curLen = len(w)
        hist[curLen] = hist.get(curLen, 0) + 1

    modeLen = None

    for k, v in hist.iteritems():
        if not modeLen or v > hist[modeLen]:
            modeLen = k

        print "{0} -> {1}".format(k, v)

    print "Mode length: {0}".format(modeLen)

if __name__ == "__main__":
    main()
