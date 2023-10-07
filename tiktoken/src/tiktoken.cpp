#include <fstream>
#include "registry.h"
#include "model.h"
#include "sys_env.h"
#include "ScopeGuard.h"
#include <boost/algorithm/string.hpp>
#include <curl/curl.h>
#include "tiktoken.h"

namespace TiktokenCpp
{
    static const char* s_encoding_names[] = {
        "r50k_base", "p50k_base", "p50k_edit", "cl100k_base"
    };

    std::vector<std::string_view> ListEncodingNames()
    {
        std::vector<std::string_view> names;
        for (auto& name : s_encoding_names)
            names.push_back(name);

        return names;
    }

    std::unique_ptr<TikToken> GetEncoding(const std::string_view& encoding_name)
    {
        const EncodingParam& param = Registry::GetEncodingParam(encoding_name);

        return std::make_unique<TikToken>(param);
    }

    std::unique_ptr<TikToken> EncodingForModel(const std::string& model_name)
    {
        std::string lowerName = boost::to_lower_copy(model_name);
        std::string_view encoding_name = Model::EncodingNameForModel(lowerName.c_str());

        return GetEncoding(encoding_name);
    }

    static size_t HttpWriteData(void* ptr, size_t size, size_t nmemb, std::ofstream* os)
    {
        try
        {
            std::size_t count = size * nmemb;
            os->write((const char*)ptr, count);

            return count;
        }
        catch (...)
        {
            return 0;
        }
    }

    bool DownloadFile(const std::string_view& url, const std::string_view& proxy, const stdfs::path& pathname)
    {
        std::ofstream file(pathname, std::ios::out);
        if (!file)
            return false;

        if (curl_global_init(CURL_GLOBAL_ALL) == CURLE_OK)
        {
            ScopeGuard guardGlobal{ []() { curl_global_cleanup(); } };
            CURL* curl = curl_easy_init();
            if (curl)
            {
                ScopeGuard guardCurl{ [&curl]() { curl_easy_cleanup(curl); } };
                curl_easy_setopt(curl, CURLOPT_URL, url.data());
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, HttpWriteData);
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, &file);
                if (!proxy.empty())
                {
                    curl_easy_setopt(curl, CURLOPT_PROXY, proxy.data());
                    curl_easy_setopt(curl, CURLOPT_HTTPPROXYTUNNEL, 1L);
                }
                if (curl_easy_perform(curl) == CURLE_OK)
                    return true;
            }
        }

        return false;
    }

    bool DownloadEncoding(const std::string_view& name, const std::optional<std::string_view> proxy)
    {
        const EncodingParam& param = Registry::GetEncodingParam(name);
        std::string filename = std::string(name) + ".tiktoken";
        stdfs::path path = GetCacheFileFullPath(filename);

        return DownloadFile(param.bpe_url, proxy.value_or(""), path);
    }
}
