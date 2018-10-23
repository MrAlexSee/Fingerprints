/*
 *** This is an implementation of a related neighborhood generation method for approximate searching.
 *** Compile as follows: g++ -Wall -pedantic -std=c++14 -DNDEBUG -O3 -I$(BOOST_DIR)$ neighborhood.cpp -o neighborhood
 */

#include <boost/format.hpp>
#include <iostream>
#include <set>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>

#include "../helpers.hpp"

using namespace fingerprints;
using namespace std;

namespace
{

const string pSeparator = "\n";
constexpr int pNIter = 10;

}

float runHammingK1(const unordered_set<string> &wordSet, const vector<string> &patterns, int nIter = 1);
float runLevenK1(const unordered_set<string> &wordSet, const vector<string> &patterns, int nIter = 1);

string getAlphabet(const unordered_set<string> &wordSet);

int main(int argc, const char **argv)
{
    if (argc != 3)
    {
        throw runtime_error("usage: ./neighborhood [dictionary path] [patterns path]");
    }

    vector<string> dict = Helpers::readWords(argv[1], pSeparator);
    vector<string> patterns = Helpers::readWords(argv[2], pSeparator);

    cout << boost::format("Read #words = %1%, #patterns = %2%") % dict.size() % patterns.size() << endl;
    unordered_set<string> wordSet(dict.begin(), dict.end());

    // Select Hamming or Levenshtein distance here.
    // float elapsedUs = runHammingK1(wordSet, patterns, pNIter);
    float elapsedUs = runLevenK1(wordSet, patterns, pNIter);

    size_t nProcessedWords = dict.size() * patterns.size();

    float elapsedPerWordNs = (1'000.0f * elapsedUs) / static_cast<float>(nProcessedWords);
    elapsedPerWordNs /= pNIter;

    cout << boost::format("Elapsed = %1%us, per word = %2%ns (#iterations = %3%)") 
        % elapsedUs % elapsedPerWordNs % pNIter << endl;
}

float runHammingK1(const unordered_set<string> &wordSet, const vector<string> &patterns, int nIter)
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

float runLevenK1(const unordered_set<string> &wordSet, const vector<string> &patterns, int nIter)
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

            // Substitutions for k = 1.
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

            // Deletions for k = 1.
            for (size_t i = 0; i < pattern.size(); ++i)
            {
                const size_t prefixSize = i + 1;
                string candidate(pattern.size() - 1, ' ');

                char *src = const_cast<char *>(candidate.c_str());

                memcpy(src, pattern.c_str(), i);
                memcpy(src + i, pattern.c_str() + prefixSize, pattern.size() - prefixSize);

                if (wordSet.find(candidate) != wordSet.end())
                {
                    nMatches += 1;
                }
            }

            // Insertions for k = 1.
            for (size_t i = 0; i < pattern.size() + 1; ++i)
            {
                string candidate(pattern.size() + 1, ' ');
                const size_t prefixSize = i + 1;

                char *src = const_cast<char *>(candidate.c_str());

                memcpy(src, pattern.c_str(), i);
                memcpy(src + prefixSize, pattern.c_str() + i, pattern.size() - i);

                for (const char c : alphabet)
                {
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
