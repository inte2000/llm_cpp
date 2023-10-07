#pragma once

#include <functional>
#include <optional>
//#include <unordered_set>
#include "global_define.h"
#include "pcre2cpp.h"

namespace TiktokenCpp
{
    class CoreBpe final
    {
    public:
        CoreBpe(std::unique_ptr<encode_dict> encoder, const StrViewToInt& specialTokensEncoder, const std::string_view& pattern);
        std::string TokenToSymbol(uint32_t token);

        std::vector<uint32_t> EncodeOrdinaryNative(const std::string& utf8Text);
        std::vector<uint32_t> EncodeOrdinaryNative(const std::wstring& utf16Text);

        std::vector<uint32_t> EncodeNative(const std::string& utf8Text, const Utf8StringSet& allowedSpecial);
        std::vector<uint32_t> EncodeNative(const std::wstring& utf16Text, const Utf8StringSet& allowedSpecial);

        std::vector<std::string> DecodeNative(const std::vector<uint32_t>& tokens);

    protected:
        std::unique_ptr<decode_dict> InitDecodeDict();
        std::vector<std::string> Utf8WordsSpliter(const std::string& utf8Text);
        //std::vector<std::u8string> Utf16WordsSpliter(const std::u16string& utf16Text);
        std::vector<uint32_t> BytePairMerge(const std::vector<uint8_t>& piece, std::function<uint32_t(std::pair<size_t, size_t>)> f);
        std::vector<uint32_t> BytePairEncode(const std::vector<uint8_t>& piece);

    private:
        std::unique_ptr<encode_dict> m_encoder;
        std::unique_ptr<decode_dict> m_decoder;
        Utf8StrToInt m_specialTokensEncoder;
        decode_dict m_specialTokensDecoder;
        Pcre2::CPcre2Regex<char> m_Regex;
        Pcre2::CPcre2Regex<char> m_SpecialRegex;
    };
}


