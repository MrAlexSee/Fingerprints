"""
Checks whether the result (number of matches) from fingerprints is the same as the desired value.
"""

pExpected = 7
pInFile = "res.txt"

def main():
    with open(pInFile, "r") as f:
        data = f.read()

    lines = data.split("\n")
    lines = [l for l in lines if l]

    nMatches = int(lines[-2].split()[-1])

    if nMatches == pExpected:
        print "OK (#matches = {0})".format(nMatches)
    else:
        print "ERROR: {0} != {1}".format(nMatches, pExpected)

if __name__ == "__main__":
    main()
