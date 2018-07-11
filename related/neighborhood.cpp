/*
 *** This is an implementation of a related neighborhood generation method for approximate searching.
 *** Compile as follows: g++ -Wall -pedantic -std=c++14 -I$(BOOST_DIR)$ neighborhood.cpp -o neighborhood
 */

#include <boost/format.hpp>
#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>

#include "../helpers.hpp"

using namespace fingerprints;
using namespace std;

namespace
{

const string separator = "\n";

}

double run(const set<string> &wordSet, const vector<string> &patterns);
string getAlphabet(const set<string> &wordSet);

int main(int argc, const char **argv)
{
    vector<string> dict = Helpers::readWords(argv[1], separator);
    vector<string> patterns = Helpers::readWords(argv[2], separator);

    set<string> wordSet(dict.begin(), dict.end());

    double elapsedUs = run(wordSet, patterns);
    cout << boost::format("Elapsed = %1%us)") % elapsedUs << endl;
}

double run(const set<string> &wordSet, const vector<string> &patterns)
{
    int nMatches = 0;
    clock_t start, end;

    string alphabet = getAlphabet(wordSet);
    cout << "Alphabet size = " << alphabet.size() << endl;

    start = std::clock();

    for (const string &pattern : patterns) 
    {
        vector<string> cands;
        cands.reserve(patterns.size() * (alphabet.size() - 1));

        // This search assumes k = 1.
        for (size_t i = 0; i < pattern.size(); ++i)
        {
            for (const char c : alphabet)
            {
                if (c == pattern[i])
                {
                    continue;
                }

                string cand = pattern.substr(0, i) + string(1, c) + pattern.substr(i + 1, string::npos);

                if (wordSet.find(cand) != wordSet.end())
                {
                    nMatches += 1;
                }
            }
        }
    }    

    end = std::clock();

    cout << "Got #matches = " << nMatches << endl;

    double elapsedS = (end - start) / static_cast<double>(CLOCKS_PER_SEC);
    return elapsedS * 1'000'000;
}

string getAlphabet(const set<string> &wordSet)
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
