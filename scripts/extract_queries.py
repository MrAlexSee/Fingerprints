"""
Extracts the queries which consists in sampling the words of a given size
from a dictionary and introducing random errors.
"""

import random

# Number of queries.
pNQueries = 33000

# If true, extracts queries of size pQuerySize, otherwise extracts queries with sizes from pQuerySizeRange.
pSingleSize = False
# Number of letters per query.
pQuerySize = 10
# Range of number of letters per query.
pQuerySizeRange = range(2, 34 + 1, 1)

# Maximum number of errors (e), number of errors will be sampled from [1, e]. Set to 0 to ignore.
pMaxNErrors = 0

# Input file path.
pInFile = "../data/dict_synth.txt"
# Output file path.
pOutFile = "queries.txt"

def readWords(inFile):
    print "Reading from: {0}".format(inFile)

    with open(inFile, "r") as f:
        words = f.read()

    words = words.split("\n")
    print "Read #words = {0}".format(len(words))

    return words

def extractQueries(words, querySize, nQueries, maxNErrors):
    words = [w for w in words if len(w) == querySize]

    if not words:
        return []

    alphabet = list(set("".join(words)))
    print "Sampled alphabet of size = {0}".format(len(alphabet))

    queries = []

    for _ in xrange(nQueries):
        base = list(random.choice(words))
        
        if maxNErrors >= 1:
            indList = list(xrange(len(base)))

            curNErrors = random.randint(1, maxNErrors)
            curIndexes = random.sample(indList, curNErrors)

            for ind in curIndexes:
                base[ind] = random.choice(alphabet)

        queries += ["".join(base)]

    return queries

def main():
    words = readWords(pInFile)

    if pSingleSize:
        print "Single query size = {0}".format(pQuerySize)
        queries = extractQueries(words, pQuerySize, pNQueries, pMaxNErrors)
    else:
        print "Query sizes = [{0}, {1}]".format(pQuerySizeRange[0], pQuerySizeRange[-1])

        queries = []
        nQueries = int(pNQueries / len(pQuerySizeRange))

        for curQuerySize in pQuerySizeRange:
            queries += extractQueries(words, curQuerySize, nQueries, pMaxNErrors)

    with open(pOutFile, "w") as f:
        f.write("\n".join(queries))

    print "Dumped #queries = {0}, max #errors = {1}, to: {2}".format(len(queries), pMaxNErrors, pOutFile)

if __name__ == "__main__":
    main()
