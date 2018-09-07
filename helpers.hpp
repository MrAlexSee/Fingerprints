#ifndef HELPERS_HPP
#define HELPERS_HPP

#include <boost/algorithm/string.hpp>
#include <fstream>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

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
    inline static size_t getTotalSize(const std::vector<std::string> &words);

    /** Leaves only words having [size] characters in the [words] vector. */
    inline void static filterWordsBySize(std::vector<std::string> &words, size_t size);

    /*
     *** FILES
     */

    inline static bool isFileReadable(const std::string &filePath);
    inline static std::vector<std::string> readWords(const std::string &filePath, const std::string &separator);

    /** Appends [text] to file with [filePath] followed by an optional newline if [newline] is true. */
    inline static void dumpToFile(const std::string &text, const std::string &filePath, bool newline = false);
    inline static bool removeFile(const std::string &filePath);

    /*
     *** RANDOM
     */

    /** Returns a set of [count] random distinct numbers from range [start] to [end] (both inclusive). */
    inline static std::set<int> randNumbersFromRange(int start, int end, int count);

    /** Returns a random alphanumeric string having [size] characters. */
    inline static std::string genRandomStringAlphNum(int size);

private:
    /** Lookup table for alphanumeric characters. */
    static constexpr const char *alnumLUT = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    /** Size of alnumLUT exluding the terminating '\0'. */
    static constexpr int alnumLUTSize = 62;
};

size_t Helpers::getTotalSize(const std::vector<std::string> &words)
{
    size_t totalSize = 0;

    for (const std::string &word : words)
    {
        totalSize += word.size();
    }

    return totalSize;
}

void Helpers::filterWordsBySize(std::vector<std::string> &words, size_t size)
{
    for (auto it = words.begin(); it != words.end(); )
    {
        if (it->size() != size)
        {
            it = words.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

bool Helpers::isFileReadable(const std::string &filePath)
{
    std::ifstream inStream(filePath);
    return inStream.good();
}

std::vector<std::string> Helpers::readWords(const std::string &filePath, const std::string &separator)
{
    using namespace std;
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

void Helpers::dumpToFile(const std::string &text, const std::string &filePath, bool newline)
{
    std::ofstream outStream(filePath, std::ios_base::app);

    if (!outStream)
    {
        throw std::runtime_error("failed to write file (insufficient permisions?): " + filePath);
    }

    outStream << text;

    if (newline)
    {
        outStream << std::endl;
    }
}

bool Helpers::removeFile(const std::string &filePath)
{
    return remove(filePath.c_str()) == 0;
}

std::set<int> Helpers::randNumbersFromRange(int start, int end, int count)
{
    using namespace std;

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

std::string Helpers::genRandomStringAlphNum(int size)
{
    using namespace std;

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
