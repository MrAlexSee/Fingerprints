#include "catch.hpp"
#include "repeat.hpp"

#include "../helpers.hpp"

using namespace std;

namespace fingerprints
{

namespace
{

constexpr int maxNStrings = 100;
constexpr int stringSize = 50;

const string tmpFileName = "fingerprints_tmp.dat";

}

TEST_CASE("is getting total size for empty vector correct", "[files]")
{
    vector<string> vec;
    REQUIRE(Helpers::getTotalSize(vec) == 0);
}

TEST_CASE("is getting total size correct", "[files]")
{
    vector<string> vec { "ala", "ma", "kota" };
    REQUIRE(Helpers::getTotalSize(vec) == 9);
}

TEST_CASE("is getting total size for randomized strings correct", "[files]")
{
    for (int nStrings = 0; nStrings < maxNStrings; ++nStrings)
    {
        vector<string> vec;
        
        for (int iStr = 0; iStr < nStrings; ++iStr)
        {
            vec.emplace_back(string(stringSize, 'a'));
        }

        REQUIRE(Helpers::getTotalSize(vec) == nStrings * stringSize);
    }
}

TEST_CASE("is reading empty words correct", "[files]")
{
    Helpers::dumpToFile("", tmpFileName, false);
    
    vector<string> words = Helpers::readWords(tmpFileName, "\n");
    REQUIRE(words.size() == 0);

    Helpers::removeFile(tmpFileName);
    REQUIRE(Helpers::isFileReadable(tmpFileName) == false);
}

TEST_CASE("is reading words correct", "[files]")
{
    string str = "ala\nma\nkota";
    Helpers::dumpToFile(str, tmpFileName, false);

    vector<string> words = Helpers::readWords(tmpFileName, "\n");
    
    REQUIRE(words.size() == 3);
    REQUIRE(words == vector<string> { "ala", "ma", "kota" });

    Helpers::removeFile(tmpFileName);
    REQUIRE(Helpers::isFileReadable(tmpFileName) == false);
}

TEST_CASE("is reading words with whitespace correct", "[files]")
{
    string str = "ala\nma  \n\n\n\n\n kota  \n";
    Helpers::dumpToFile(str, tmpFileName, false);

    vector<string> words = Helpers::readWords(tmpFileName, "\n");
    
    REQUIRE(words.size() == 3);
    REQUIRE(words == vector<string> { "ala", "ma", "kota" });

    Helpers::removeFile(tmpFileName);
    REQUIRE(Helpers::isFileReadable(tmpFileName) == false);
}

} // namespace fingerprints
