#ifndef HELPERS_HPP
#define HELPERS_HPP

#include <boost/algorithm/string.hpp>
#include <fstream>
#include <sstream>
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

    static size_t getTotalSize(const vector<string> &words);

    /*
     *** FILES
     */

    inline static bool isFileReadable(const string &filePath);
    static vector<string> readWords(const string &filePath, const string &separator);
    
    static void dumpToFile(const string &text, const string &filePath, bool newline = false);
    inline static bool removeFile(const string &filePath);
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

} // namespace fingerprints

#endif // HELPERS_HPP
