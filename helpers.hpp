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
 
    /*
     *** FILES
     */

    static bool isFileReadable(const string &filePath);
    static vector<string> readWords(const string &filePath, const string &separator);
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

} // namespace fingerprints

#endif // HELPERS_HPP
