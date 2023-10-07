#pragma once

#include <string>
#include <vector>


//output utf-16 string with BOM header
[[nodiscard]]
std::wstring UTF16StrFromLocalMBCS(const std::string& input);

//output GBK string from utf-16 string (must has BOM header)
[[nodiscard]]
std::string LocalMBCSFromUTF16Str(const std::wstring& input);

[[nodiscard]]
std::string UTF8StrFromLocalMBCS(const std::string& input);

[[nodiscard]]
std::string LocalMBCSFromUTF8Str(const std::string& input);

[[nodiscard]]
std::wstring UTF16LEStrFromLocalMBCS(const std::string& input);

[[nodiscard]]
std::string LocalMBCSFromUTF16LEStr(const std::wstring& input);


[[nodiscard]]
std::wstring UTF16LEStrFromUTF8(const std::string& input);

[[nodiscard]]
std::string UTF8FromUTF16LEStr(const std::wstring& input);
