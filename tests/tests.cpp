#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest/doctest.h"

#include "../Coalested Hashtable/StaticCoalestedHashtable.hpp"

using namespace algo;

#include <unordered_set>
#include <string>
#include <limits>
#include <random>

template <typename Integer = int>
auto random_integer(Integer min = std::numeric_limits<int>::min(),
                    Integer max = std::numeric_limits<int>::max()) -> Integer
{
    static std::mt19937 gen {std::random_device{}()};
    std::uniform_int_distribution<Integer> dist {min, max};
    return dist(gen);
}

TEST_CASE("Static Coalested Hashtable")
{
    auto elems = std::unordered_set<int>{};
    auto table = StaticCoalestedHashTable<int, 100>{};

    for (auto i = 0; i < decltype(table)::max_size(); ++i) {
        REQUIRE(table.size() == elems.size());
        REQUIRE(std::is_permutation(table.begin(), table.end(), elems.begin()));

        const auto to_insert = random_integer();
        REQUIRE(*table.insert(to_insert) == to_insert);
        elems.insert(to_insert);

        REQUIRE (table.find(to_insert) != table.end());
    }

    for (auto i = 0; i < decltype(table)::max_size(); ++i) {
        REQUIRE(table.size() == elems.size());
        REQUIRE(std::is_permutation(table.begin(), table.end(), elems.begin()));

        const auto to_erase = *table.begin();

        table.erase(to_erase);
        elems.erase(to_erase);

        REQUIRE (table.find(to_erase) == table.end());
    }
}