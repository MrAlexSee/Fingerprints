import random

nQueries = 10000 # Number of queries.
querySize = 69 # Size of each query (#chars).
maxNErrors = 2 # Maximum number of errors (e), will be sampled from [1, e].

inFile = "../data/dict_urls.txt" # Input file path.
outFile = "queries.txt" # Output file path.

def readWords(inFile, wordSize):
    print "Reading from: {0}".format(inFile)

    with open(inFile, "r") as f:
        words = f.read()

    words = words.split()
    print "Read #words = {0}".format(len(words))

    words = [w for w in words if len(w) == wordSize]
    print "Filtered #words = {0} of size = {1}".format(len(words), wordSize)

    return words

def extractQueries(words, nQueries):
    alphabet = list(set("".join(words)))
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
    words = readWords(inFile, querySize)
    queries = extractQueries(words, nQueries)

    with open(outFile, "w") as f:
        f.write("\n".join(queries))

    print "Dumped #queries = {0}, max #errors = {1}, to: {2}".format(nQueries, maxNErrors, outFile)

if __name__ == "__main__":
    main()
