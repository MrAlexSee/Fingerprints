"""
Performs approximate Hamming distance matching for a dictionary and patterns
using a naive algorithm.
"""

# Input dictionary file path.
pInDictFile = "../data/dict_iamerican_insane.txt"
# Input patterns file path.
pInPatternsFile = "../data/queries_iamerican_insane_8.txt"

# Number of patterns, set to -1 to ignore.
pNPatterns = 50
# Number of mismatches.
pK = 1 

def readWords(inFile):
    print "Reading from: {0}".format(inFile)

    with open(inFile, "r") as f:
        words = f.read()

    words = words.split("\n")
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
    words = readWords(pInDictFile)
    patterns = readWords(pInPatternsFile)

    if pNPatterns > 0:
        patterns = patterns[ : pNPatterns]

    nTotalMatches = 0

    for iP, pattern in enumerate(patterns):
        print "\nPattern: {0} ({1}/{2})".format(pattern, iP + 1, len(patterns))
        matches = set()

        for iW, word in enumerate(words):
            if len(pattern) == len(word) and calcHamming(pattern, word) <= pK:
                matches.add((word, iW))

        print "#matches = {0}".format(len(matches))
        
        if len(matches):
            print "Matches: {0}".format(list(matches))

        nTotalMatches += len(matches)

    print "Total #matches = {0} for k = {1}".format(nTotalMatches, pK)

if __name__ == "__main__":
    main()
