#pragma once

#include <string_view>


namespace TiktokenCpp 
{
    class Model final
    {
    public:
        static std::string_view EncodingNameForModel(const std::string_view& model_name);
    };
}