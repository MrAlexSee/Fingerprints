/*
 *** This is an implementation of a related neighborhood generation method for approximate searching.
 *** Compile as follows: g++ -Wall -pedantic -std=c++14 -DNDEBUG -O3 -I$(BOOST_DIR)$ neighborhood.cpp -o neighborhood
 */

#include <boost/format.hpp>
#include <iostream>
#include <set>
#include <string>
#include <unordered_set>
#include <vector>

#include "../helpers.hpp"

using namespace fingerprints;
using namespace std;

namespace
{

const string pSeparator = "\n";
int pNIter = 1;

}

float run(const unordered_set<string> &wordSet, const vector<string> &patterns, int nIter = 1);
string getAlphabet(const unordered_set<string> &wordSet);

int main(int argc, const char **argv)
{
    vector<string> dict = Helpers::readWords(argv[1], pSeparator);
    vector<string> patterns = Helpers::readWords(argv[2], pSeparator);

    cout << boost::format("Read #words = %1%, #patterns = %2%") % dict.size() % patterns.size() << endl;
    unordered_set<string> wordSet(dict.begin(), dict.end());

    float elapsedUs = run(wordSet, patterns, pNIter);

    size_t processedWordsCount = dict.size() * patterns.size();
    double elapsedPerWordNs = (1'000.0f * elapsedUs) / static_cast<float>(processedWordsCount);

    elapsedPerWordNs /= pNIter;
    cout << boost::format("Elapsed = %1%us, per word = %2%ns") % elapsedUs % elapsedPerWordNs << endl;
}

float run(const unordered_set<string> &wordSet, const vector<string> &patterns, int nIter)
{
    int nMatches = 0;
    clock_t start, end;

    string alphabet = getAlphabet(wordSet);
    cout << "Alphabet size = " << alphabet.size() << endl;

    start = std::clock();

    for (int i = 0; i < nIter; ++i)
    {
        for (const string &pattern : patterns) 
        {
            // Checking for an exact match.
            if (wordSet.find(pattern) != wordSet.end())
            {
                nMatches += 1;
            }

            // This search assumes k = 1.
            for (size_t i = 0; i < pattern.size(); ++i)
            {
                for (const char c : alphabet)
                {
                    if (pattern[i] == c)
                    {
                        continue;
                    }

                    string candidate = pattern;
                    candidate[i] = c;

                    if (wordSet.find(candidate) != wordSet.end())
                    {
                        nMatches += 1;
                    }
                }
            }
        }    
    }

    end = std::clock();

    cout << "Got #matches = " << nMatches << endl;

    float elapsedS = (end - start) / static_cast<float>(CLOCKS_PER_SEC);
    return elapsedS * 1'000'000.0f;
}

string getAlphabet(const unordered_set<string> &wordSet)
{
    set<char> alph;

    for (const string &word : wordSet)
    {
        for (const char c : word)
        {
            alph.insert(c);
        }
    }

    string res;

    for (const char c : alph)
    {
        res += c;
    }

    return res;
}
