"""
Extracts the queries which consists in sampling the words of a given size
from a dictionary and introducing random errors.
"""

import random

# Number of queries.
pNQueries = 10000

# Size of each query (#chars).
pQuerySize = 69
# Maximum number of errors (e), number of errors will be sampled from [1, e].
pMaxNErrors = 2

# Input file path.
pInFile = "../data/dict_urls.txt"
# Output file path.
pOutFile = "queries.txt"

def readWords(inFile, wordSize):
    print "Reading from: {0}".format(inFile)

    with open(inFile, "r") as f:
        words = f.read()

    words = words.split("\n")
    print "Read #words = {0}".format(len(words))

    words = [w for w in words if len(w) == wordSize]
    print "Filtered #words = {0} of size = {1}".format(len(words), wordSize)

    return words

def extractQueries(words, nQueries, maxNErrors):
    alphabet = list(set("".join(words)))
    print "Sampled alphabet of size = {0}".format(len(alphabet))

    queries = []

    for _ in xrange(nQueries):
        base = list(random.choice(words))
        indList = list(xrange(len(base)))
        
        curNErrors = random.randint(1, maxNErrors)
        curIndexes = random.sample(indList, curNErrors)

        for ind in curIndexes:
            base[ind] = random.choice(alphabet)

        queries += ["".join(base)]

    return queries

def main():
    words = readWords(pInFile, pQuerySize)
    queries = extractQueries(words, pNQueries, pMaxNErrors)

    with open(pOutFile, "w") as f:
        f.write("\n".join(queries))

    print "Dumped #queries = {0}, max #errors = {1}, to: {2}".format(pNQueries, pMaxNErrors, pOutFile)

if __name__ == "__main__":
    main()
