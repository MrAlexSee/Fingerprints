inDictFile = "../data/dict_iamerican_insane.txt" # Input dictionary file path.
inPatternsFile = "../data/queries_iamerican_insane_8.txt" # Input patterns file path.

k = 1 # Number of mismatches.
nPatterns = 50 # Number of patterns, set to -1 to ignore.

def readWords(inFile):
    print "Reading from: {0}".format(inFile)

    with open(inFile, "r") as f:
        words = f.read()

    words = words.split()
    print "Read #words = {0}".format(len(words))

    return words

def calcHamming(str1, str2):
    assert len(str1) == len(str2)
    nErrors = 0

    for i in xrange(len(str1)):
        if str1[i] != str2[i]:
            nErrors += 1
    
    return nErrors

def main():
    words = readWords(inDictFile)
    patterns = readWords(inPatternsFile)

    if nPatterns > 0:
        patterns = patterns[ : nPatterns]

    nTotalMatches = 0

    for iP, pattern in enumerate(patterns):
        print "Pattern: {0} ({1}/{2})".format(pattern, iP + 1, len(patterns))
        matches = set()

        for iW, word in enumerate(words):
            if len(pattern) == len(word) and calcHamming(pattern, word) <= k:
                matches.add(iW)

        print "Matches: {0}".format(matches)
        print "#matches = {0}\n".format(len(matches))

        nTotalMatches += len(matches)

    print "Total #matches = {0}".format(nTotalMatches)

if __name__ == "__main__":
    main()
