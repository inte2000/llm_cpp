#pragma once

#include <string_view>
#include <vector>
#include "registry.h"
#include "model.h"
#include "global_define.h"

namespace TiktokenCpp
{
    class CoreBpe;

    class TikToken final
    {
    public:
        TikToken(const EncodingParam& param);
        TikToken(const TikToken& token) = delete;
        ~TikToken();
        TikToken& operator=(const TikToken& token) = delete;

        std::vector<uint32_t> EncodeOrdinary(const std::string& utf8Text);
        std::vector<uint32_t> Encode(const std::string& utf8Text,
                                    StringSetUnion allowedSpecial = StringSet{},
                                    StringSetUnion disallowedSpecial = "all");
        
        std::string Decode(const std::vector<uint32_t>& tokens);
        std::string TokenToSymbol(uint32_t token) const;
        std::vector<std::string> TokenToSymbols(const std::vector<uint32_t>& tokens) const;
        
        std::string_view GetName() const { return m_name; }
    protected:
    private:
        std::unique_ptr<CoreBpe> m_corebpe;
        StringSet m_SpecialTokensSet;
        std::uint32_t m_maxTokenValue = 0;
        std::string_view m_name;
    };

}

