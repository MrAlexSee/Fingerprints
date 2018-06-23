#ifndef HELPERS_HPP
#define HELPERS_HPP

#include <cmath>
#include <cstdio>
#include <fstream>
#include <random>
#include <stdexcept>
#include <string>
#include <sstream>
#include <type_traits>

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
    static string readFile(const string &filePath);

    static void dumpToFile(const string &text, const string &filePath, bool newline = false);
    static bool removeFile(const string &filePath);
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

inline string Helpers::readFile(const string &filePath)
{
    ifstream inStream(filePath);

    if (!inStream)
    {
        throw runtime_error("failed to read file (insufficient permisions?): " + filePath);
    }

    return static_cast<stringstream const&>(stringstream() << inStream.rdbuf()).str();
}

inline void Helpers::dumpToFile(const string &text, const string &filePath, bool newline)
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

inline bool Helpers::removeFile(const string &filePath)
{
    return remove(filePath.c_str()) == 0;
}

} // namespace fingerprints

#endif // HELPERS_HPP
