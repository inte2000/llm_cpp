#include "iconv.h"
#include "Utf8String.h"
#include <cassert>
#include <algorithm>
#include <iterator>


const std::size_t LOCAL_BUF_SIZE = 64;

template<typename CT = char>
std::basic_string<CT> UTFStrFromLocalMBCS(const char *coder, const std::string& input)
{
    std::basic_string<CT> output;
    char szTmp[LOCAL_BUF_SIZE] = { 0 };
    const char* pSource = input.c_str();
    std::size_t inRemain = input.length();

    iconv_t cd = iconv_open(coder, "");
    if (reinterpret_cast<unsigned long long>(cd) == 0xFFFFFFFF)
        return output;

    output.reserve(inRemain * 2); 
    while (inRemain > 0)
    {
        char* pTmp = szTmp;
        std::size_t outRemain = LOCAL_BUF_SIZE;
        std::size_t rtn = iconv(cd, (char**)&pSource, &inRemain, &pTmp, &outRemain);
        if (outRemain == LOCAL_BUF_SIZE)
            break;

        output.append((CT*)szTmp, (CT*)pTmp);
        //memset(szTmp, 0, 64);
    }
    iconv_close(cd);

    return output;
}

template<typename CT = char>
std::string LocalMBCSFromUTFStr(const char* coder, const std::basic_string<CT>& input)
{
    char szTmp[LOCAL_BUF_SIZE] = { 0 };
    const CT* pSource = input.c_str();
    std::size_t inRemain = input.length() * sizeof(CT);
    std::string output;

    iconv_t cd = iconv_open("", coder);
    if (reinterpret_cast<unsigned long long>(cd) == 0xFFFFFFFF)
        return output;

    output.reserve(inRemain * 2);
    while (inRemain > 0)
    {
        char* pTmp = szTmp;
        std::size_t outRemain = LOCAL_BUF_SIZE;
        std::size_t rtn = iconv(cd, (char**)&pSource, &inRemain, &pTmp, &outRemain);
        if (outRemain == LOCAL_BUF_SIZE)
            break;

        output.append(szTmp, pTmp);
    }
    iconv_close(cd);

    return output;
}

std::wstring UTF16StrFromLocalMBCS(const std::string& input)
{
    auto rtn = UTFStrFromLocalMBCS<wchar_t>("UTF-16", input);

    return rtn;
}

std::string LocalMBCSFromUTF16Str(const std::wstring& input)
{
    std::string rtn = LocalMBCSFromUTFStr<wchar_t>("UTF-16", input);

    return rtn;
}

std::string UTF8StrFromLocalMBCS(const std::string& input)
{
    auto rtn = UTFStrFromLocalMBCS<char>("UTF-8", input);

    return rtn;
}

std::string LocalMBCSFromUTF8Str(const std::string& input)
{
    std::string rtn = LocalMBCSFromUTFStr<char>("UTF-8", input);

    return rtn;
}

std::wstring UTF16LEStrFromLocalMBCS(const std::string& input)
{
    std::wstring rtn = UTFStrFromLocalMBCS<wchar_t>("UTF-16LE", input);

    return rtn;
}

std::string LocalMBCSFromUTF16LEStr(const std::wstring& input)
{
    std::string rtn = LocalMBCSFromUTFStr<wchar_t>("UTF-16LE", input);

    return rtn;
}

std::wstring UTF16LEStrFromUTF8(const std::string& input)
{
    std::wstring output;
    char szTmp[LOCAL_BUF_SIZE] = { 0 };
    const char* pSource = input.c_str();
    std::size_t inRemain = input.length();

    iconv_t cd = iconv_open("UTF-16LE", "UTF-8");
    if (reinterpret_cast<unsigned long long>(cd) == 0xFFFFFFFF)
        return output;

    output.reserve(inRemain * 2);
    while (inRemain > 0)
    {
        char* pTmp = szTmp;
        std::size_t outRemain = LOCAL_BUF_SIZE;
        iconv(cd, (char**)&pSource, &inRemain, &pTmp, &outRemain);
        if (outRemain == LOCAL_BUF_SIZE)
            break;

        output.append((wchar_t*)szTmp, (wchar_t*)pTmp);
    }
    iconv_close(cd);

    return output;
}

std::string UTF8FromUTF16LEStr(const std::wstring& input)
{
    char szTmp[LOCAL_BUF_SIZE] = { 0 };
    const wchar_t* pSource = input.c_str();
    std::size_t inRemain = input.length() * sizeof(char16_t);
    std::string output;

    iconv_t cd = iconv_open("UTF-8", "UTF-16LE");
    if (reinterpret_cast<unsigned long long>(cd) == 0xFFFFFFFF)
        return output;

    output.reserve(inRemain * 2);
    while (inRemain > 0)
    {
        char* pTmp = szTmp;
        std::size_t outRemain = LOCAL_BUF_SIZE;
        iconv(cd, (char**)&pSource, &inRemain, &pTmp, &outRemain);
        if (outRemain == LOCAL_BUF_SIZE)
            break;

        output.append(szTmp, pTmp);
    }
    iconv_close(cd);

    return output;
}