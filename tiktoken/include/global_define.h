#pragma once

#include <string_view>
#include <vector>
#include <variant>
#include <set>
#include <unordered_map>

template<>
struct std::hash<std::vector<uint8_t>>
{
    //FNV hash µÄ±äÌå
    std::size_t operator()(const std::vector<uint8_t>& va) const noexcept
    {
        int p = 16777619;
        std::size_t hash = 2166136261L;
        for (std::size_t i = 0; i < va.size(); i++)
        {
            hash = (hash ^ va[i]) * p;
        }

        hash += hash << 13;
        hash ^= hash >> 7;
        hash += hash << 3;
        hash ^= hash >> 17;
        hash += hash << 5;

        return hash;
    }
};

namespace TiktokenCpp
{
    using Utf8StringSet = std::set<std::string>;
    using StringSet = std::set<std::string>;
    using StringSetUnion = std::variant<std::string_view, StringSet>;

    using Utf8StrToInt = std::unordered_map<std::string, int32_t>;
    using StrViewToInt = std::unordered_map<std::string_view, int32_t>;

    using encode_dict = std::unordered_map<std::vector<uint8_t>, int32_t>;
    using decode_dict = std::unordered_map<int32_t, std::string>;

}

