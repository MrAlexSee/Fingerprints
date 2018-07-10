/*
 *** This is an implementation of a related neighborhood generation method for approximate searching.
 *** Compile as follows: g++ -Wall -pedantic -std=c++14 neighborhood.cpp -o neighborhood
 */

#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>

#include "helpers.hpp"

using namespace fingerprints;
using namespace std;

namespace
{

const string separator = "\n";
constexpr int k = 1;

}

double run(const set<string> &wordSet, const vector<string> &patterns);

int main(int argc, const char **argv)
{
    vector<string> dict = Helpers::readWords(argv[1], separator);
    vector<string> patterns = Helpers::readWords(argv[2], separator);

    set<string> wordSet(dict.begin(), dict.end());

    double elapsedUs = run(wordSet, patterns);
    cout << "Elapsed = " << elapsedUs << "us" << endl;
}

double run(const set<string> &wordSet, const vector<string> &patterns)
{
    int nMatches = 0;
    clock_t start, end;

    const char *neighborhood;

    start = std::clock();

    for (const string &pattern : patterns) 
    {
        // 1. We generate the neighborhood.

        // 2. We search for all words from the neighborhood.
    }    

    end = std::clock();

    cout << "Got #matches = " << nMatches << endl;

    double elapsedS = (end - start) / static_cast<double>(CLOCKS_PER_SEC);
    return elapsedS * 1'000'000;
}
