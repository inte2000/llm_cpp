#pragma once

#include <string>
#include <optional>
#include "global_define.h"


namespace TiktokenCpp 
{
    typedef struct tagEncodingParam
    {
        std::string_view name;
        std::optional<uint32_t> vocab_n;
        std::string_view pat_str;
        std::string_view bpe_url;
        StrViewToInt special_tokens;
    }EncodingParam;

    class Registry final
    {
    public:
        static const EncodingParam& GetEncodingParam(const std::string_view& encoding_name);
    };
}