import string

inFile = "english200.txt" # Input file path.
outFile = "dict.txt" # Output file path.

def isWordOK(s):
    return len(s) == len([c for c in s if c in string.printable])

def readWordsFilt(inFile):
    print "Reading from: {0}".format(inFile)

    with open(inFile, "r") as f:
        words = f.read()

    words = words.split()
    print "Read #words = {0}".format(len(words))

    words = set(words)
    print "Unique #words = {0}".format(len(words))

    wordsFilt = [w for w in words if isWordOK(w)]
    print "Filtered #words = {0}".format(len(wordsFilt))

    return wordsFilt

def main():
    wordsFilt = readWordsFilt(inFile)

    nChars = len(set("".join(wordsFilt)))
    print "Alphabet size = {0}".format(nChars)

    wordsFilt.sort()

    with open(outFile, "w") as f:
        f.write("\n".join(wordsFilt))

    print "Dumped to: {0}".format(outFile)

if __name__ == "__main__":
    main()
