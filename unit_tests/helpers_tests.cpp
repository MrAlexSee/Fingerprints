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

constexpr int nRandomRepeats = 100;

const string tmpFileName = "fingerprints_tmp.dat";

}

TEST_CASE("is getting total size for empty vector correct", "[collections]")
{
    vector<string> vec;
    REQUIRE(Helpers::getTotalSize(vec) == 0);
}

TEST_CASE("is getting total size correct", "[collections]")
{
    vector<string> vec { "ala", "ma", "kota" };
    REQUIRE(Helpers::getTotalSize(vec) == 9);
}

TEST_CASE("is getting total size for repeated strings correct", "[collections]")
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

TEST_CASE("is filtering empty words by size correct", "[collections]")
{
    vector<string> empty;

    for (size_t size = 0; size < static_cast<size_t>(stringSize); ++size)
    {
        Helpers::filterWordsBySize(empty, size);
        REQUIRE(empty.size() == 0);
    }
}

TEST_CASE("is filtering words by size correct", "[collections]")
{
    vector<string> vec { "ala", "ma", "kota", "a", "jarek", "ma", "psa" };

    vector<string> vec0(vec);
    Helpers::filterWordsBySize(vec0, 0);
    REQUIRE(vec0.size() == 0);

    vector<string> vec1(vec);
    Helpers::filterWordsBySize(vec1, 1);
    REQUIRE(vec1.size() == 1);

    vector<string> vec2(vec);
    Helpers::filterWordsBySize(vec2, 2);
    REQUIRE(vec2.size() == 2);

    vector<string> vec3(vec);
    Helpers::filterWordsBySize(vec3, 3);
    REQUIRE(vec3.size() == 2);

    vector<string> vec4(vec);
    Helpers::filterWordsBySize(vec4, 4);
    REQUIRE(vec4.size() == 1);

    vector<string> vec5(vec);
    Helpers::filterWordsBySize(vec5, 5);
    REQUIRE(vec5.size() == 1);

    vector<string> vec6(vec);
    Helpers::filterWordsBySize(vec6, 6);
    REQUIRE(vec6.size() == 0);
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

TEST_CASE("is reading words with semicolon separator correct", "[files]")
{
    string str = "ala;ma;kota";
    Helpers::dumpToFile(str, tmpFileName, false);

    vector<string> words = Helpers::readWords(tmpFileName, ";");

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

TEST_CASE("is getting random numbers from range correct", "[random]")
{
    repeat(nRandomRepeats, [] {
        for (int count = 1; count <= 10; ++count)
        {
            set<int> range = Helpers::randNumbersFromRange(0, 100, count);
            REQUIRE(range.size() == count);

            for (const int n : range)
            {
                REQUIRE(n >= 0);
                REQUIRE(n <= 100);
            }
        }
    });
}

TEST_CASE("does random generation from bad range throw", "[random]")
{
    REQUIRE_THROWS_AS(Helpers::randNumbersFromRange(10, -5, 1), invalid_argument);
    REQUIRE_THROWS_AS(Helpers::randNumbersFromRange(0, 2, 4), invalid_argument);
}

} // namespace fingerprints
