#pragma once

#include <string>
#include <string_view>
#include <optional>
#include <memory>
#include "global_define.h"


namespace TiktokenCpp
{
    //load a encoding file by path name
    std::unique_ptr<encode_dict> LoadTiktokenBpe(const std::string& pathname);

    //load a encoding file by encoding name (get file from local cache)
    std::unique_ptr<encode_dict> GetTiktokenEncoding(const std::string_view& name);

    //process escape char replacement
    std::string EscapeRegex(const std::string& str);

    std::vector<uint8_t> StringToBytes(const std::string& str);
    std::string BytesToString(const std::vector<uint8_t>& bytes);
    Utf8StringSet Utf8StrsetFromStrSet(const StringSet& strSet);

    template<typename T>
    std::optional<decltype(T::token)> BinarySearch(const std::vector<T>& container, int32_t target)
    {
        std::size_t left = 0, right = container.size();
        while (left < right)
        {
            std::size_t mid = left + (right - left) / 2;
            if (container[mid].value == target) {
                return container[mid].token;
            }
            else if (container[mid].value > target) {
                right = mid;
            }
            else if (container[mid].value < target) {
                left = mid + 1;
            }
        }
        return decltype(T::token){};
    }

}

