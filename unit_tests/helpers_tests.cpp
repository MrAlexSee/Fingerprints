#include "catch.hpp"
#include "repeat.hpp"

#include "../helpers.hpp"

using namespace std;

namespace fingerprints
{

TEST_CASE("is getting total size for empty vector correct", "[files]")
{
    vector<string> vec;
    REQUIRE(Helpers::getTotalSize(vec) == 0);
}

TEST_CASE("is reading words correct", "[files]")
{

}

} // namespace fingerprints
