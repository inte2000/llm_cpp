#include "sys_env.h"


namespace TiktokenCpp
{
    static SystemInit s_sysInit;

    void InitSystemEnv()
    {
        stdfs::path curpath = stdfs::current_path();
        curpath /= "cache";
        if (!stdfs::exists(curpath))
        {
            stdfs::create_directories(curpath);
        }

        stdfs::path tmppath = stdfs::temp_directory_path();
        tmppath /= "llm_cpp";
        if (!stdfs::exists(tmppath))
        {
            stdfs::create_directories(tmppath);
        }
    }

    stdfs::path GetCacheFileFullPath(const std::string_view& filename)
    {
        stdfs::path curpath = stdfs::current_path();
        curpath /= "cache";
        curpath /= filename;

        return curpath;
    }

    std::string GetCachePathName()
    {
        stdfs::path curpath = stdfs::current_path();
        curpath /= "cache";

        return curpath.string();
        /*
        char workingpath[260] = { 0 };
        getcwd(workingpath, sizeof(workingpath));
        return std::string(workingpath);
        */
    }

    std::string GetTempPathName()
    {
        stdfs::path curpath = stdfs::temp_directory_path();
        curpath /= "llm_cpp";

        return curpath.string();
    }

}