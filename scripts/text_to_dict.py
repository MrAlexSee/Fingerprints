"""
Transforms input text to a dictionary, splitting words on any whitespace.
"""

import string

# Input file path.
pInFile = "english200.txt"
# Output file path.
pOutFile = "dict.txt"

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
    wordsFilt = readWordsFilt(pInFile)

    nChars = len(set("".join(wordsFilt)))
    print "Alphabet size = {0}".format(nChars)

    wordsFilt.sort()

    with open(pOutFile, "w") as f:
        f.write("\n".join(wordsFilt))

    print "Dumped to: {0}".format(pOutFile)

if __name__ == "__main__":
    main()
