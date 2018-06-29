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

TEST_CASE("is reading words correct", "[files]")
{

}

} // namespace fingerprints
