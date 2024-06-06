#pragma once

#include <string_view>
#include <cassert>
#include <stdexcept>
#ifndef NO_CPP20_FORMAT
    #include <format>
#else
    #include <boost/format.hpp>
#endif
#include <vector>
#include <optional>

#define PCRE2_CODE_UNIT_WIDTH 8
#include "pcre2.h"

namespace Pcre2
{

    const uint32_t Pcre2MakeGeneralContextFail = 201;
    const uint32_t Pcre2CompileFail = 202;
    const uint32_t Pcre2CreateMatchDataFail = 203;
    const uint32_t Pcre2MakeCompileContextFail = 204;
    const uint32_t Pcre2MakeMatchContextFail = 205;

    class CPcre2Exception : public std::runtime_error
    {
    public:
        CPcre2Exception(uint32_t errCode, const char* msg) : std::runtime_error(msg)
        {
            m_errCode = errCode;
            m_errOffset = 0;
        }
        CPcre2Exception(uint32_t errCode) : std::runtime_error("General PCRE2 error!")
        {
            m_errCode = errCode;
            m_errOffset = 0;
        }
        CPcre2Exception(uint32_t errCode, std::size_t offset) : std::runtime_error("Pattern compile error!")
        {
            m_errCode = errCode;
            m_errOffset = offset;
        }

        [[nodiscard]] 
        char const* what() const noexcept override
        {
#ifndef NO_CPP20_FORMAT
            return std::format("Pcre2 error: {0}, error code: {1}, error offset: {2}", 
                std::exception::what(), m_errCode, m_errOffset).c_str();
#else
            boost::format whatFmt("Pcre2 error: %1%, error code: %2%, error offset: %3%");
            whatFmt % std::exception::what() % m_errCode % m_errOffset;
            return whatFmt.str().c_str();
#endif        
        }

        uint32_t GetErrorCode() const { return m_errCode; }
        std::size_t GetErrorOffset() const { return m_errOffset; }
    private:
        uint32_t m_errCode;
        std::size_t m_errOffset;
    };

    class CPcre2CompileContext;
    class CPcre2MatchContext;

    class CPcre2GeneralContext final
    {
    public:
        CPcre2GeneralContext() : m_context(nullptr) {}
        CPcre2GeneralContext(void* memory_data);
        CPcre2GeneralContext(const CPcre2GeneralContext& ctx);
        CPcre2GeneralContext(CPcre2GeneralContext&& ctx) noexcept = default;
        ~CPcre2GeneralContext() noexcept;
        CPcre2GeneralContext& operator=(const CPcre2GeneralContext& ctx);
        CPcre2GeneralContext& operator=(CPcre2GeneralContext&& ctx) noexcept = default;
        CPcre2CompileContext CreateCompileContext();
        CPcre2MatchContext CreateMatchContext();

        bool IsEmpty() const { return (m_context == nullptr); }
        operator pcre2_general_context* () const { return m_context; }
        operator pcre2_general_context* () { return m_context; }
    private:
        void Swap(CPcre2GeneralContext& ctx) noexcept;
        pcre2_general_context* m_context;
    };

    class CPcre2MatchContext final
    {
    public:
        //CPcre2MatchContext() : m_context(nullptr) {}
        CPcre2MatchContext(bool bInit = false, pcre2_general_context* gcontext = nullptr);
        CPcre2MatchContext(pcre2_match_context* ctx);
        CPcre2MatchContext(const CPcre2MatchContext& ctx);
        CPcre2MatchContext(CPcre2MatchContext&& ctx) noexcept = default;
        ~CPcre2MatchContext() noexcept;
        CPcre2MatchContext& operator=(const CPcre2MatchContext& ctx);
        CPcre2MatchContext& operator=(CPcre2MatchContext&& ctx) noexcept = default;

        int SetMatchLimit(uint32_t value) 
        {
            assert(m_context != nullptr);
            return pcre2_set_match_limit(m_context, value);
        }
        
        bool IsEmpty() const { return (m_context == nullptr); }
        operator pcre2_match_context* () const { return m_context; }
        operator pcre2_match_context* () { return m_context; }
    private:
        void Swap(CPcre2MatchContext& ctx) noexcept;
        pcre2_match_context* m_context;
    };

    class CPcre2CompileContext final
    {
    public:
        CPcre2CompileContext() : m_context(nullptr) {}
        CPcre2CompileContext(pcre2_general_context* gcontext);
        CPcre2CompileContext(pcre2_compile_context* ctx);
        CPcre2CompileContext(const CPcre2CompileContext& ctx);
        CPcre2CompileContext(CPcre2CompileContext&& ctx) noexcept = default;
        ~CPcre2CompileContext() noexcept;
        CPcre2CompileContext& operator=(const CPcre2CompileContext& ctx);
        CPcre2CompileContext& operator=(CPcre2CompileContext&& ctx) noexcept = default;

        /*
          PCRE2_EXTRA_ALLOW_LOOKAROUND_BSK     Allow \K in lookarounds PCRE2_EXTRA_ALLOW_SURROGATE_ESCAPES  Allow \x{df800} to \x{dfff}
                                                 in UTF-8 and UTF-32 modes
          PCRE2_EXTRA_ALT_BSUX                 Extended alternate \u, \U, and \x handling
          PCRE2_EXTRA_BAD_ESCAPE_IS_LITERAL    Treat all invalid escapes as a literal following character
          PCRE2_EXTRA_ESCAPED_CR_IS_LF         Interpret \r as \n
          PCRE2_EXTRA_MATCH_LINE               Pattern matches whole lines
          PCRE2_EXTRA_MATCH_WORD               Pattern matches "words"        
        */
        int SetExtraOptions(uint32_t extra_options) 
        {
            assert(m_context != nullptr);
            return pcre2_set_compile_extra_options(m_context, extra_options);
        }
        int SetRecursionGuard(int (*guard_function)(uint32_t, void*), void* user_data)
        {
            assert(m_context != nullptr);
            return pcre2_set_compile_recursion_guard(m_context, guard_function, user_data);
        }

        bool IsEmpty() const { return (m_context == nullptr); }
        operator pcre2_compile_context* () const { return m_context; }
        operator pcre2_compile_context* () { return m_context; }
    private:
        void Swap(CPcre2CompileContext& ctx) noexcept;
        pcre2_compile_context* m_context;
    };

    const CPcre2CompileContext s_emptyCompileCtx;
    const CPcre2MatchContext s_emptyMatchCtx;
    const CPcre2GeneralContext s_emptyGeneralCtx;

    struct Pcre2Match
    {
        PCRE2_SIZE start;
        PCRE2_SIZE end;
    };

    //only for range-based for loop
    template<typename T>
    class CPcre2OVector_Iterator final 
    {
    public:
        CPcre2OVector_Iterator(PCRE2_SIZE* overtor, uint32_t index) : m_overtor(overtor), m_curIdx(index)
        {}
        
        bool operator!=(const CPcre2OVector_Iterator<T>& iter) const 
        { 
            return m_curIdx != iter.m_curIdx; 
        }
        T operator*() 
        { 
            return T{ m_overtor[2 * m_curIdx], m_overtor[2 * m_curIdx + 1] };
        }
        CPcre2OVector_Iterator<T>& operator++() 
        { 
            m_curIdx++;
            return *this;
        }
    private:
        PCRE2_SIZE* m_overtor;
        uint32_t m_curIdx;
    };

    class CPcre2OVector
    {
    public:
        CPcre2OVector(PCRE2_SIZE* vec, uint32_t count) : m_overtor(vec), m_count(count)
        { }
        CPcre2OVector_Iterator<Pcre2Match> begin() const
        {
            return { m_overtor, 0 };  
        }
        CPcre2OVector_Iterator<Pcre2Match> end() const
        {
            return { m_overtor, m_count };  
        }
        Pcre2Match First() const
        {
            return { m_overtor[0], m_overtor[1] };
        }
        Pcre2Match Last() const
        {
            return { m_overtor[2 * (m_count - 1)], m_overtor[2 * (m_count - 1) + 1] };
        }
        Pcre2Match operator [](uint32_t idx) const 
        {
            return { m_overtor[2 * idx], m_overtor[2 * idx + 1] };
        }
        uint32_t GetCount() const { return m_count; }
    private:
        PCRE2_SIZE* m_overtor;
        uint32_t m_count;
    };

    class CPcre2MatchData
    {
    public:
        CPcre2MatchData(pcre2_match_data* md) : m_matchData(md) {}
        CPcre2MatchData(uint32_t ovecsize, std::optional<CPcre2GeneralContext> ctx = std::nullopt);
        CPcre2MatchData(const CPcre2MatchData& md) = delete;
        CPcre2MatchData(CPcre2MatchData&& md) noexcept = default;
        CPcre2MatchData& operator=(const CPcre2MatchData& md) = delete;
        CPcre2MatchData& operator=(CPcre2MatchData&& md) noexcept = default;
        ~CPcre2MatchData() noexcept;

        CPcre2OVector GetOVectorPointer()
        {
            assert(m_matchData != nullptr);
            return CPcre2OVector(pcre2_get_ovector_pointer(m_matchData), pcre2_get_ovector_count(m_matchData));
        }
        PCRE2_SIZE* GetRawOVector() const { return pcre2_get_ovector_pointer(m_matchData); }
        uint32_t GetOVectorCount()
        {
            assert(m_matchData != nullptr);
            return pcre2_get_ovector_count(m_matchData);
        }
        PCRE2_SIZE GetMatchDataSize() const
        {
            assert(m_matchData != nullptr);
            return pcre2_get_match_data_size(m_matchData);
        }

        operator pcre2_match_data* () const { return m_matchData; }
        operator pcre2_match_data* () { return m_matchData; }
    private:
        pcre2_match_data* m_matchData;
    };

    template<typename Ch = char8_t> //default for utf-8
    class CPcre2Regex
    {
        typedef std::basic_string<Ch>  StringType;
    public:
        CPcre2Regex() : m_code(nullptr) {}
        CPcre2Regex(pcre2_code* code) : m_code(code) {}
        CPcre2Regex(const CPcre2Regex& re) = delete;
        CPcre2Regex(CPcre2Regex&& re) noexcept = default;
        CPcre2Regex(const std::string& pattern,
                    std::size_t option = PCRE2_UTF|PCRE2_UCP, //default options for unicode string
                    std::optional<CPcre2CompileContext> ctx = std::nullopt)
        {
            Compile(pattern, option, ctx);
        }

        CPcre2Regex& operator=(const CPcre2Regex& re) = delete;
        CPcre2Regex& operator=(CPcre2Regex&& re) noexcept = default;

        void Compile(const std::string& pattern,
            std::size_t option = PCRE2_UTF|PCRE2_UCP,  //PCRE2_ZERO_TERMINATED
            std::optional<CPcre2CompileContext> ctx = std::nullopt)
        {
            int err_code = 0;
            PCRE2_SIZE err_offset = 0;
            //PCRE2_SPTR _pat = (PCRE2_SPTR)pattern;
            PCRE2_SIZE length = pattern.length();
            pcre2_code* code = pcre2_compile((PCRE2_SPTR)pattern.c_str(), length, (uint32_t)option, &err_code,
                &err_offset, ctx.value_or(s_emptyCompileCtx));//
            if (code == nullptr)
                throw CPcre2Exception(err_code, err_offset);

            m_code = code;
        }

        std::optional<Pcre2Match> Match(const StringType& text, PCRE2_SIZE startoffset = 0, uint32_t options = 0)
        {
            assert(m_code != nullptr);

            CPcre2MatchData matchData = CreateMatchDataFromPattern();
            PCRE2_SPTR subject = (PCRE2_SPTR)text.c_str();
            PCRE2_SIZE length = text.length();
            int rtn = pcre2_match(m_code, subject, length, startoffset, options, matchData, nullptr);
            if (rtn > 0)
            {
                PCRE2_SIZE* overtor = matchData.GetRawOVector();
                return Pcre2Match{ overtor[0], overtor[1] };
            }

            return std::nullopt;
        }

        std::vector<StringType> Matchs(const StringType& text, uint32_t options = 0, std::optional<CPcre2MatchContext> ctx = std::nullopt)
        {
            assert(m_code != nullptr);

            std::vector<StringType> matchs;
            CPcre2MatchData matchData = CreateMatchDataFromPattern();
            PCRE2_SPTR subject = (PCRE2_SPTR)text.c_str();
            PCRE2_SIZE length = text.length();
            PCRE2_SIZE start_offset = 0;
            int rc = pcre2_match(m_code, subject, length, start_offset, options, matchData, ctx.value_or(s_emptyMatchCtx));
            while (rc > 0)
            {
                PCRE2_SIZE* overtor = matchData.GetRawOVector();
                int i = 0;
                for (i = 0; i < rc; i++)
                {
                    matchs.emplace_back(text.substr(overtor[2 * i], overtor[2 * i + 1] - overtor[2 * i]));
                }
                start_offset = overtor[2 * (i - 1) + 1];
                rc = pcre2_match(m_code, subject, length, start_offset, options, matchData, ctx.value_or(s_emptyMatchCtx));
            }

            return matchs;
        }

        int Match(const StringType& text, PCRE2_SIZE startoffset, CPcre2MatchData& match_data, uint32_t options = 0, std::optional<CPcre2MatchContext> ctx = std::nullopt)
        {
            assert(m_code != nullptr);

            PCRE2_SPTR subject = (PCRE2_SPTR)text.c_str();
            PCRE2_SIZE length = text.length();

            return pcre2_match(m_code, subject, length, startoffset, options, match_data, ctx.value_or(s_emptyMatchCtx));
        }

        CPcre2MatchData CreateMatchDataFromPattern(std::optional<CPcre2GeneralContext> ctx = std::nullopt)
        {
            assert(m_code != nullptr);

            pcre2_match_data* match_data = pcre2_match_data_create_from_pattern(m_code, ctx.value_or(s_emptyGeneralCtx));

            return CPcre2MatchData(match_data);
        }

    private:
        pcre2_code* m_code;
    };

}