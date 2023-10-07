#pragma once

#include <string_view>
#include <filesystem>

namespace stdfs = std::filesystem;

namespace TiktokenCpp
{
    void InitSystemEnv();
    stdfs::path GetCacheFileFullPath(const std::string_view& filename);
    std::string GetCachePathName();
    std::string GetTempPathName();

    class SystemInit final
    {
    public:
        SystemInit()
        {
            InitSystemEnv();
        }
    };
}


