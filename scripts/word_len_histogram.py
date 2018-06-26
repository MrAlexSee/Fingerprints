inFile = "../data/dict_iamerican_insane.txt" # Input file path.

def readWords(inFile):
    print "Reading from: {0}".format(inFile)

    with open(inFile, "r") as f:
        words = f.read()

    words = words.split()
    print "Read #words = {0}".format(len(words))

    return words

def main():
    words = readWords(inFile)

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
