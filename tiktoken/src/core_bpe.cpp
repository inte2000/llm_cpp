#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/replace.hpp>
#include "utils.h"
#include "Utf8String.h"
#include "core_bpe.h"
#include "pcre2cpp.h"


namespace TiktokenCpp
{
    CoreBpe::CoreBpe(std::unique_ptr<encode_dict> encoder, const StrViewToInt& specialTokensEncoder, const std::string_view& pattern)
    {
        m_encoder = std::move(encoder);
        m_Regex.Compile(pattern.data());

        m_specialTokensEncoder.reserve(specialTokensEncoder.size());
        std::for_each(specialTokensEncoder.begin(), specialTokensEncoder.end(),
            [this](const auto& mi) { m_specialTokensEncoder.emplace(UTF8StrFromLocalMBCS(mi.first.data()), mi.second); });

        std::vector<std::string> parts(specialTokensEncoder.size());
        std::transform(specialTokensEncoder.begin(), specialTokensEncoder.end(), parts.begin(),
            [](const auto& mi) { return EscapeRegex(mi.first.data()); });
        m_SpecialRegex.Compile(boost::join(parts, "|"));

        m_specialTokensDecoder.reserve(m_specialTokensEncoder.size());
        std::for_each(m_specialTokensEncoder.begin(), m_specialTokensEncoder.end(),
            [this](const auto& mi) { m_specialTokensDecoder.emplace(mi.second, mi.first); });
    }

    std::string CoreBpe::TokenToSymbol(uint32_t token)
    {
        if (m_decoder == nullptr)
            m_decoder = InitDecodeDict();
        
        std::string symbol;
        auto it = m_decoder->find(token);
        if (it != m_decoder->end())
        {
            symbol = it->second;
        }

        return symbol;
    }

    std::vector<uint32_t> CoreBpe::EncodeOrdinaryNative(const std::string& utf8Text)
    {
        std::vector<uint32_t> tokens;
        std::vector<std::string> words = Utf8WordsSpliter(utf8Text);
        for (const auto& word : words)
        {
            std::vector<uint8_t> word_bytes = StringToBytes(word);
            //std::size_t hash = std::hash<std::vector<uint8_t>>()(word_bytes);
            auto it = m_encoder->find(word_bytes);
            if (it != m_encoder->end())
            {
                tokens.push_back(it->second);
            }
            else
            {
                std::vector<uint32_t> word_tokens = BytePairEncode(word_bytes);
                /*
                tokens.insert(tokens.end(), std::make_move_iterator(word_tokens.begin()),
                    std::make_move_iterator(word_tokens.end()));
                */
                std::move(word_tokens.begin(), word_tokens.end(), std::back_inserter(tokens));
            }
        }

        return tokens;
    }

    //todo: 
    std::vector<uint32_t> CoreBpe::EncodeOrdinaryNative(const std::wstring& utf16Text)
    {
        std::vector<uint32_t> tokens;

        return tokens;
    }

    std::optional<std::tuple<std::size_t, std::size_t, std::string>>
    FindFirstSpecialPosition(const std::string& utf8Text, const Utf8StringSet& allowedSpecial, const Pcre2::CPcre2OVector& overtor)
    {
        for (const auto& mat : overtor)
        {
            if (allowedSpecial.contains(utf8Text.substr(mat.start, mat.end - mat.start)))
            {
                return std::tuple{ mat.start, mat.end, utf8Text.substr(mat.start, mat.end - mat.start) };
            }
        }

        return std::nullopt;
    }

    std::vector<uint32_t> CoreBpe::EncodeNative(const std::string& utf8Text, const Utf8StringSet& allowedSpecial)
    {
        std::vector<uint32_t> tokens;
        
        std::size_t start = 0;
        while (true)
        {
            Pcre2::CPcre2MatchData nextSpecial = m_SpecialRegex.CreateMatchDataFromPattern();
            std::size_t startFind = start;
            std::optional<std::tuple<std::size_t, std::size_t, std::string>> SpecialIdx = std::nullopt;
            while (true)
            {
                int rc = m_SpecialRegex.Match(utf8Text, startFind, nextSpecial);
                if (rc <= 0) 
                    break;

                Pcre2::CPcre2OVector overtor = nextSpecial.GetOVectorPointer();
                SpecialIdx = FindFirstSpecialPosition(utf8Text, allowedSpecial, overtor);
                if (SpecialIdx)
                    break;

                startFind = overtor.Last().start + 1;
            }
            std::size_t end = (SpecialIdx) ? std::get<0>(SpecialIdx.value()) : utf8Text.length();

            std::vector<std::string> words = Utf8WordsSpliter(utf8Text.substr(start, end - start));
            for (const auto& word : words)
            {
                std::vector<uint8_t> word_bytes = StringToBytes(word);
                auto it = m_encoder->find(word_bytes);
                if (it != m_encoder->end())
                {
                    tokens.push_back(it->second);
                }
                else
                {
                    std::vector<uint32_t> word_tokens = BytePairEncode(word_bytes);
                    std::move(word_tokens.begin(), word_tokens.end(), std::back_inserter(tokens));
                }
            }

            if (SpecialIdx)
            {
                tokens.push_back(m_specialTokensEncoder[std::get<2>(SpecialIdx.value())]);
                start = std::get<1>(SpecialIdx.value());
            }
            else
            {
                break;
            }
        }

        return tokens;
    }

    //todo: 
    std::vector<uint32_t> CoreBpe::EncodeNative(const std::wstring& utf16Text, const Utf8StringSet& allowedSpecial)
    {
        std::vector<uint32_t> tokens;
        
        return tokens;
    }

    std::vector<std::string> CoreBpe::DecodeNative(const std::vector<uint32_t>& tokens)
    {
        if (m_decoder == nullptr)
            m_decoder = InitDecodeDict();

        std::vector<std::string> words;
        words.reserve(tokens.size());
        for (const uint32_t& token : tokens)
        {
            auto it = m_decoder->find(token);
            if (it != m_decoder->end())
            {
                words.emplace_back(it->second);
            }
            else
            {
                auto specIt = m_specialTokensDecoder.find(token);
                if (specIt != m_specialTokensDecoder.end())
                {
                    words.emplace_back(specIt->second);
                }
            }
        }
        
        return words;
    }

    std::unique_ptr<decode_dict> CoreBpe::InitDecodeDict()
    {
        auto ptr = std::make_unique<decode_dict>();
        std::for_each(m_encoder->begin(), m_encoder->end(),
            [&ptr](const auto& enc) { ptr->emplace(enc.second, BytesToString(enc.first)); });

        return ptr;
    }

    std::vector<std::string> CoreBpe::Utf8WordsSpliter(const std::string& utf8Text)
    {
        std::vector<std::string> tokens;

        Pcre2::CPcre2MatchData matchData = m_Regex.CreateMatchDataFromPattern();
        PCRE2_SIZE start_offset = 0;
        int rc = m_Regex.Match(utf8Text, start_offset, matchData);
        while (rc > 0)
        {
            Pcre2::CPcre2OVector overtor = matchData.GetOVectorPointer();
            for (const auto& mat : overtor)
            {
                tokens.push_back(utf8Text.substr(mat.start, mat.end - mat.start));
            }
            start_offset = overtor.Last().end;
            rc = m_Regex.Match(utf8Text, start_offset, matchData);
        }

        return tokens;
    }

    static std::vector<std::pair<size_t, size_t>> InitParts(std::size_t size)
    {
        std::vector<std::pair<size_t, size_t>> parts;
        for (std::size_t i = 0; i < size + 1; i++)
            parts.emplace_back(i, std::numeric_limits<size_t>::max());

        return parts;
    }

    std::vector<uint32_t> CoreBpe::BytePairMerge(const std::vector<uint8_t>& piece, std::function<uint32_t(std::pair<size_t, size_t>)> f)
    {
        std::vector<std::pair<size_t, size_t>> parts = InitParts(piece.size());

        auto get_rank = [&](const std::vector<std::pair<size_t, size_t>>& parts, size_t start_idx, size_t skip) -> std::optional<size_t>
        {
            if (start_idx + skip + 2 < parts.size())
            {
                auto it = m_encoder->find(std::vector<uint8_t>(piece.begin() + parts[start_idx].first, piece.begin() + parts[start_idx + skip + 2].first));
                if (it != m_encoder->end())
                {
                    return it->second;
                }
            }
            return std::nullopt;
        };

        for (size_t i = 0; i < parts.size() - 2; ++i)
        {
            auto rank = get_rank(parts, i, 0);
            if (rank.has_value())
            {
                parts[i].second = *rank;
            }
        }

        while (parts.size() > 1)
        {
            size_t min_rank = std::numeric_limits<size_t>::max();
            size_t min_rank_idx = 0;

            for (size_t i = 0; i < parts.size() - 1; ++i)
            {
                [[maybe_unused]] auto [_, rank] = parts[i];
                if (rank < min_rank)
                {
                    min_rank = rank;
                    min_rank_idx = i;
                }
            }

            if (min_rank != std::numeric_limits<size_t>::max())
            {
                size_t i = min_rank_idx;
                parts[i].second = get_rank(parts, i, 1).value_or(std::numeric_limits<size_t>::max());
                if (i > 0)
                {
                    parts[i - 1].second = get_rank(parts, i - 1, 1).value_or(std::numeric_limits<size_t>::max());
                }

                parts.erase(parts.begin() + i + 1);
            }
            else
            {
                break;
            }
        }

        std::vector<uint32_t> out;
        for (size_t i = 0; i < parts.size() - 1; ++i)
        {
            out.push_back(f({ parts[i].first, parts[i + 1].first }));
        }

        return out;
    }

    std::vector<uint32_t> CoreBpe::BytePairEncode(const std::vector<uint8_t>& piece)
    {
        if (piece.size() == 1) {
            return { m_encoder->at(piece) };
        }

        return BytePairMerge(piece, [&](const std::pair<size_t, size_t>& p)
            { return m_encoder->at(std::vector<uint8_t>(piece.begin() + p.first, piece.begin() + p.second)); });
    }
}
