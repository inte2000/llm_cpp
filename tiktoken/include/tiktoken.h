#pragma once

#include "token_encoding.h"


namespace TiktokenCpp
{
    using AllowedSpecial = StringSet;
    using DisallowedSpecial = StringSet;
    
    //list current encoding
    std::vector<std::string_view> ListEncodingNames();

    //get an encoding object by encoding name
    std::unique_ptr<TikToken> GetEncoding(const std::string_view& encoding_name);

    //get an encoding object by model name
    std::unique_ptr<TikToken> EncodingForModel(const std::string& model_name);

    //download encoding to local cache,
    //proxy support: http://127.0.0.1:8080, or https://127.0.0.1:8081, or socks5://127.0.0.1:8089 
    bool DownloadEncoding(const std::string_view& name, const std::optional<std::string_view> proxy = std::nullopt);

}

