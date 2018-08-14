"""
Performs approximate (Hamming or Levenshtein) matching for a dictionary and patterns.
"""

from nltk.metrics import edit_distance

# Input dictionary file path.
pInDictFile = "../data/dict_iamerican_insane.txt"
# Input patterns file path.
pInPatternsFile = "../data/queries_iamerican_insane_8.txt"

# Set to true to use Hamming distance, set to false to use Levenshtein distance.
pUseHamming = True

# Number of patterns, set to -1 to ignore.
pNPatterns = 3
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

def calcLeven(str1, str2):
    return edit_distance(str1, str2)

def isMatch(word, pattern, k, useHamming):
    if useHamming:
        return len(word) == len(pattern) and calcHamming(word, pattern) <= pK
    else:
        return calcLeven(pattern, word) <= pK

def main():
    print "Using distance: Ham = {0}".format(pUseHamming)

    words = readWords(pInDictFile)
    patterns = readWords(pInPatternsFile)

    if pNPatterns > 0:
        patterns = patterns[ : pNPatterns]

    nTotalMatches = 0

    for iP, pattern in enumerate(patterns):
        print "\nTesting pattern: {0} ({1}/{2})".format(pattern, iP + 1, len(patterns))
        matches = set()

        for iW, word in enumerate(words):
            if isMatch(word, pattern, pK, pUseHamming):
                matches.add((word, iW))

        print "#matches = {0}".format(len(matches))
        
        if matches:
            print "Matches: {0}".format(list(matches))

        nTotalMatches += len(matches)

    print "\nTotal #matches = {0} for #patterns = {1} for k = {2}".format(nTotalMatches, len(patterns), pK)

if __name__ == "__main__":
    main()
