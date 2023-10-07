#ifndef NO_CPP20_FORMAT
    #include <format>
#else
    #include <boost/format.hpp>
#endif
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/replace.hpp>
#include "Utf8String.h"
#include "error_handler.h"
#include "pcre2cpp.h"
#include "core_bpe.h"
#include "utils.h"
#include "token_encoding.h"


namespace TiktokenCpp
{
    //get max token value from special tikens
    std::size_t GetMaxSpecialTokenValue(const StrViewToInt& tokens)
    {
        std::size_t value = std::numeric_limits<std::size_t>::min();
        for (const auto& tk : tokens)
        {
            if (tk.second > value)
                value = tk.second;
        }

        return value;
    }

    //get max token value from encoding map
    std::size_t GetMaxTokenValue(const encode_dict* pDict)
    {
        assert(pDict != nullptr);
        std::size_t value = pDict->size();
        if (value != 0)
            value -= 1;
        return value;
    }

    Pcre2::CPcre2Regex<char> SpecialTokenRegex(const StringSet& tokens)
    {
        std::vector<std::string> strVec(tokens.size());
        /*
        std::transform(tokens.begin(), tokens.end(), strVec.begin(),
            [](const std::string& str) { return boost::replace_all_copy(str, "\\", "\\\\"); });
        */
        std::transform(tokens.begin(), tokens.end(), strVec.begin(),
            [](const std::string& str) { return EscapeRegex(str); });
#ifndef NO_CPP20_FORMAT
        return Pcre2::CPcre2Regex<char>(std::format("({})", boost::join(strVec, "|")));
#else
        boost::format regexFmt("(%1%)");
        regexFmt % boost::join(strVec, "|");
        return Pcre2::CPcre2Regex<char>(regexFmt.str());
#endif
    }

    TikToken::TikToken(const EncodingParam& param)
    {
        std::unique_ptr<encode_dict> encoder = GetTiktokenEncoding(param.name);
        if(encoder == nullptr)
            ThrowGeneralException("local cache not find encoding file, please download encoding file first. encoding name: ", param.name);

        m_name = param.name;
        m_maxTokenValue = std::max(GetMaxTokenValue(encoder.get()), GetMaxSpecialTokenValue(param.special_tokens));
        if (param.vocab_n.has_value())
        {
            assert((param.special_tokens.size() + encoder->size()) == param.vocab_n.value());
            assert(m_maxTokenValue == (param.vocab_n.value() - 1));
        }

        for_each(param.special_tokens.begin(), param.special_tokens.end(),
            [this](const auto& spi) {m_SpecialTokensSet.insert(spi.first.data()); });

        m_corebpe = std::make_unique<CoreBpe>(std::move(encoder), param.special_tokens, param.pat_str);
    }

    TikToken::~TikToken()
    {
    }

    std::vector<int32_t> TikToken::EncodeOrdinary(const std::string& utf8Text)
    {
        try 
        {
            return m_corebpe->EncodeOrdinaryNative(utf8Text);
        }
        catch (const UnicodeEncoderException& e)
        {
            //text = text.encode("utf-16", "surrogatepass").decode("utf-16", "replace")
            std::wstring utf16Text = UTF16LEStrFromUTF8(utf8Text);
            return m_corebpe->EncodeOrdinaryNative(utf16Text);
        }
    }

    std::vector<int32_t> TikToken::Encode(const std::string& utf8Text,
                                          StringSetUnion allowedSpecial, StringSetUnion disallowedSpecial)
    {
        StringSet allowedSpecialSet, disallowedSpecialSet; //null set
        if ((allowedSpecial.index() == 0) && (std::get<0>(allowedSpecial) == "all"))
            allowedSpecialSet = m_SpecialTokensSet;
        else
            allowedSpecialSet = std::move(std::get<1>(allowedSpecial));
        
        if ((disallowedSpecial.index() == 0) && (std::get<0>(disallowedSpecial) == "all"))
        {
            StringSet tmp;
            std::set_difference(m_SpecialTokensSet.begin(), m_SpecialTokensSet.end(), 
                                allowedSpecialSet.begin(), allowedSpecialSet.end(),
                                std::inserter(tmp, tmp.begin()));
            disallowedSpecialSet = std::move(tmp);
        }
        else
            disallowedSpecialSet = std::move(std::get<1>(disallowedSpecial));

        if (disallowedSpecialSet.size() > 0)
        {
            Pcre2::CPcre2Regex<char> regex = SpecialTokenRegex(disallowedSpecialSet);
            if (regex.Match(utf8Text))
            {
                const  char* ValueError = "Encountered text corresponding to disallowed special token {token!r}.\n"
                    "If you want this text to be encoded as a special token, "
                    "pass it to `allowed_special`, e.g. `allowed_special={{{token!r}, ...}}`.\n"
                    "If you want this text to be encoded as normal text, disable the check for this token "
                    "by passing `disallowed_special=(enc.special_tokens_set - {{{token!r}}})`.\n"
                    "To disable this check for all special tokens, pass `disallowed_special=()`.\n";

                throw std::runtime_error(ValueError);
            }
        }

        try
        {
            return m_corebpe->EncodeNative(utf8Text, Utf8StrsetFromStrSet(allowedSpecialSet));
        }
        catch (const UnicodeEncoderException& e)
        {
            //text = text.encode("utf-16", "surrogatepass").decode("utf-16", "replace")
            std::wstring utf16Text = UTF16LEStrFromUTF8(utf8Text);
            return m_corebpe->EncodeNative(utf16Text, Utf8StrsetFromStrSet(allowedSpecialSet));
        }
    }

    std::string TikToken::Decode(const std::vector<int32_t>& tokens)
    {
        std::string result;
        result.reserve(tokens.size() * 8);

        auto strVec = m_corebpe->DecodeNative(tokens);
        for (const auto& str : strVec)
            result += str;

        return result;
    }

    std::string TikToken::TokenToSymbol(int32_t token) const
    { 
        return m_corebpe->TokenToSymbol(token);
    }

    std::vector<std::string> TikToken::TokenToSymbols(const std::vector<int32_t>& tokens) const
    {
        std::vector<std::string> result;
        result.reserve(tokens.size());

        auto strVec = m_corebpe->DecodeNative(tokens);
        for (const auto& str : strVec)
            result.emplace_back(str);

        return result;

    }

}

