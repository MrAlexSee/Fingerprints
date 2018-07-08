#ifndef HELPERS_HPP
#define HELPERS_HPP

#include <boost/algorithm/string.hpp>
#include <fstream>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;

namespace fingerprints
{

class Helpers
{
public:
    Helpers() = delete;

    /*
     *** COLLECTIONS
     */

    /** Returns the total size of all strings from [words]. */
    inline static size_t getTotalSize(const vector<string> &words);

    /*
     *** FILES
     */

    inline static bool isFileReadable(const string &filePath);
    inline static vector<string> readWords(const string &filePath, const string &separator);

    /** Appends [text] to file with [filePath] followed by an optional newline if [newline] is true. */
    inline static void dumpToFile(const string &text, const string &filePath, bool newline = false);
    inline static bool removeFile(const string &filePath);

    /*
     *** RANDOM
     */

    /** Returns a set of [count] random distinct numbers from range [start] to [end] (both inclusive). */
    inline static set<int> randNumbersFromRange(int start, int end, int count);

    /** Returns a random alphanumeric string having [size] characters. */
    inline static string genRandomStringAlphNum(int size);

private:
    /** Lookup table for alphanumeric characters. */
    static constexpr const char *alnumLUT = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    /** Size of alnumLUT exluding the terminating '\0'. */
    static constexpr int alnumLUTSize = 62;
};

size_t Helpers::getTotalSize(const vector<string> &words)
{
    size_t totalSize = 0;

    for (const string &word : words)
    {
        totalSize += word.size();
    }

    return totalSize;
}

bool Helpers::isFileReadable(const string &filePath)
{
    ifstream inStream(filePath);
    return inStream.good();
}

vector<string> Helpers::readWords(const string &filePath, const string &separator)
{
    ifstream inStream(filePath);

    if (!inStream)
    {
        throw runtime_error("failed to read file (insufficient permisions?): " + filePath);
    }

    string text = static_cast<stringstream const&>(stringstream() << inStream.rdbuf()).str();

    vector<string> words;
    boost::split(words, text, boost::is_any_of(separator));

    vector<string> filt;

    for (string &word : words)
    {
        boost::trim(word);

        if (word.empty() == false) 
        {
            filt.emplace_back(move(word));
        }
    }

    return filt;
}

void Helpers::dumpToFile(const string &text, const string &filePath, bool newline)
{
    ofstream outStream(filePath, ios_base::app);

    if (!outStream)
    {
        throw runtime_error("failed to write file (insufficient permisions?): " + filePath);
    }

    outStream << text;

    if (newline)
    {
        outStream << endl;
    }
}

bool Helpers::removeFile(const string &filePath)
{
    return remove(filePath.c_str()) == 0;
}

set<int> Helpers::randNumbersFromRange(int start, int end, int count)
{
    if (end - start + 1 < count)
    {
        throw invalid_argument("range is too narrow");
    }

    set<int> res;

    random_device rd;
    mt19937 mt(rd());
    uniform_int_distribution<int> dist(start, end);

    while (static_cast<int>(res.size()) < count)
    {
        res.insert(dist(mt));
    }

    return res;
}

string Helpers::genRandomStringAlphNum(int size)
{
    random_device rd;
    mt19937 mt(rd());
    uniform_int_distribution<int> dist(0, alnumLUTSize - 1);

    string res = "";

    for (int i = 0; i < size; ++i)
    {
        res += alnumLUT[dist(mt)];
    }

    return res;
}

} // namespace fingerprints

#endif // HELPERS_HPP
