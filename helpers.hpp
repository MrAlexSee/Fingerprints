#ifndef HELPERS_HPP
#define HELPERS_HPP

#include <boost/algorithm/string.hpp>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

namespace fingerprints
{

struct Helpers
{
    Helpers() = delete;

    /*
     *** COLLECTIONS
     */

    template<typename T>
    static void calcStatsMedian(const vector<T> &throughputVec, T *throughputMedian);
 
    static size_t getTotalSize(const vector<string> &words);

    /*
     *** FILES
     */

    static bool isFileReadable(const string &filePath);
    static vector<string> readWords(const string &filePath, const string &separator);
    
    static void dumpToFile(const string &text, const string &filePath, bool newline = false);
};

template<typename T>
inline void Helpers::calcStatsMedian(const vector<T> &throughputVec, T *throughputMedian)
{
    if (throughputVec.size() == 0)
    {
        return;
    }

    vector<T> tmp = throughputVec;

    sort(tmp.begin(), tmp.end());
    *throughputMedian = tmp[tmp.size() / 2];
}

size_t Helpers::getTotalSize(const vector<string> &words)
{
    size_t totalSize = 0;

    for (const string &word : words)
    {
        totalSize += word.size();
    }

    return totalSize;
}

inline bool Helpers::isFileReadable(const string &filePath)
{
    ifstream inStream(filePath);
    return inStream.good();
}

inline vector<string> Helpers::readWords(const string &filePath, const string &separator)
{
    ifstream inStream(filePath);

    if (!inStream)
    {
        throw runtime_error("failed to read file (insufficient permisions?): " + filePath);
    }

    string text = static_cast<stringstream const&>(stringstream() << inStream.rdbuf()).str();

    vector<string> words;
    boost::split(words, text, boost::is_any_of(separator));

    for (string &word : words)
    {
        boost::trim(word);
    }

    return words;
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

} // namespace fingerprints

#endif // HELPERS_HPP
