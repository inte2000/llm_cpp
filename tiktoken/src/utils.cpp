#include <fstream>
#include <regex>
#include <stdexcept>
#include <cstring>
#include <cppcodec/base64_rfc4648.hpp>
#include "Utf8String.h"
#include "sys_env.h"
#include "registry.h"
#include "utils.h"


namespace TiktokenCpp
{
    using base64 = cppcodec::base64_rfc4648;

    std::unique_ptr<encode_dict> LoadTiktokenBpe(const std::string& pathname)
    {
        std::unique_ptr<encode_dict> dict = std::make_unique<encode_dict>();
        std::ifstream file(pathname, std::ios::in);
        std::string tmpline;
        while (!file.eof())
        {
            std::getline(file, tmpline);
            if (tmpline.empty())
                break;

            auto space_pos = tmpline.find(' ', 4); //base64 编码最少 4 个字符
            if (space_pos == std::string::npos)
                throw std::runtime_error("invaid token encoding file format!");
            
            std::vector<uint8_t> token = base64::decode(tmpline.substr(0, space_pos));
            int32_t rank = std::stoi(tmpline.substr(space_pos + 1));
            dict->emplace(std::move(token), rank);
        }

        return dict;
    }

    std::unique_ptr<encode_dict> GetTiktokenEncoding(const std::string_view& name)
    {
        std::string filename = std::string(name) + ".tiktoken";
        stdfs::path path = GetCacheFileFullPath(filename);
        if (stdfs::exists(path))
        {
            return LoadTiktokenBpe(path.string());
        }

        return nullptr;
    }

    std::string EscapeRegex(const std::string& str)
    {
        std::regex specialChars{ R"([-[\]{}()*+?.,\^$|#\s])" };
        return std::regex_replace(str, specialChars, R"(\$&)");
    }

    std::vector<uint8_t> StringToBytes(const std::string& str)
    {
        std::vector<uint8_t> bytes(str.length());

        assert(sizeof(uint8_t) == sizeof(std::string::value_type));
        memcpy(bytes.data(), str.c_str(), str.length());
        bytes.resize(str.length());

        return bytes;
    }

    std::string BytesToString(const std::vector<uint8_t>& bytes)
    {
        assert(sizeof(uint8_t) == sizeof(std::string::value_type));
        std::string str(bytes.begin(), bytes.end());

        return str;
    }

    Utf8StringSet Utf8StrsetFromStrSet(const StringSet& strSet)
    {
        Utf8StringSet utf8Set;
        for (const auto& si : strSet)
            utf8Set.emplace(UTF8StrFromLocalMBCS(si));

        return utf8Set;
    }

}
