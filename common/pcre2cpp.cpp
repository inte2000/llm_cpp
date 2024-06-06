#include <cassert>
#include "pcre2cpp.h"

namespace Pcre2
{

    CPcre2GeneralContext::CPcre2GeneralContext(void* memory_data)
    {
        pcre2_general_context* copyCtx = pcre2_general_context_create(nullptr, nullptr, memory_data);
        if (copyCtx == nullptr)
            throw CPcre2Exception(1);

        m_context = copyCtx;
    }
    CPcre2GeneralContext::CPcre2GeneralContext(const CPcre2GeneralContext& ctx)
    {
        if (!ctx.IsEmpty())
        {
            pcre2_general_context *copyCtx = pcre2_general_context_copy(ctx);
            if (copyCtx == nullptr)
                throw CPcre2Exception(Pcre2MakeGeneralContextFail, "Fail to copy from other general context");

            m_context = copyCtx;
       }
        else
            m_context = nullptr;
    }
    CPcre2GeneralContext::~CPcre2GeneralContext() noexcept
    {
        if (m_context != nullptr)
            pcre2_general_context_free(m_context);
    }
    CPcre2GeneralContext& CPcre2GeneralContext::operator=(const CPcre2GeneralContext& ctx)
    {
        if (this != &ctx)
        {
            CPcre2GeneralContext tmp(ctx);
            Swap(tmp);
        }

        return *this;
    }
    CPcre2CompileContext CPcre2GeneralContext::CreateCompileContext()
    {
        pcre2_compile_context* ctx = pcre2_compile_context_create(m_context);

        return CPcre2CompileContext(ctx);
    }
    CPcre2MatchContext CPcre2GeneralContext::CreateMatchContext()
    {
        pcre2_match_context* ctx = pcre2_match_context_create(m_context);

        return CPcre2MatchContext(ctx);
    }
    void CPcre2GeneralContext::Swap(CPcre2GeneralContext& ctx) noexcept
    {
        std::swap(m_context, ctx.m_context);
    }

    CPcre2MatchContext::CPcre2MatchContext(bool bInit, pcre2_general_context* gcontext)
    {
        if (bInit)
        {
            pcre2_match_context* copyCtx = pcre2_match_context_create(gcontext);
            if (copyCtx == nullptr)
                throw CPcre2Exception(Pcre2MakeMatchContextFail);

            m_context = copyCtx;
        }
        else
            m_context = nullptr;
    }
    CPcre2MatchContext::CPcre2MatchContext(pcre2_match_context* ctx)
    {
        if (ctx == nullptr)
            throw CPcre2Exception(Pcre2MakeMatchContextFail, "attach a null match context");

        m_context = ctx;
    }
    CPcre2MatchContext::CPcre2MatchContext(const CPcre2MatchContext& ctx)
    {
        if (!ctx.IsEmpty())
        {
            pcre2_match_context* copyCtx = pcre2_match_context_copy(ctx);
            if (copyCtx == nullptr)
                throw CPcre2Exception(Pcre2MakeMatchContextFail, "Fail to copy from other match context");

            m_context = copyCtx;
        }
        else
            m_context = nullptr;
    }
    CPcre2MatchContext::~CPcre2MatchContext() noexcept
    {
        if (m_context != nullptr)
            pcre2_match_context_free(m_context);
    }
    CPcre2MatchContext& CPcre2MatchContext::operator=(const CPcre2MatchContext & ctx)
    {
        if (this != &ctx)
        {
            CPcre2MatchContext tmp(ctx);
            Swap(tmp);
        }

        return *this;
    }
    void CPcre2MatchContext::Swap(CPcre2MatchContext& ctx) noexcept
    {
        std::swap(m_context, ctx.m_context);
    }




    CPcre2CompileContext::CPcre2CompileContext(pcre2_general_context* gcontext)
    {
        pcre2_compile_context* copyCtx = pcre2_compile_context_create(gcontext);
        if (copyCtx == nullptr)
            throw CPcre2Exception(Pcre2MakeCompileContextFail);

        m_context = copyCtx;
    }
    CPcre2CompileContext::CPcre2CompileContext(pcre2_compile_context* ctx)
    {
        if (ctx == nullptr)
            throw CPcre2Exception(Pcre2MakeCompileContextFail, "attach a null context");

        m_context = ctx;
    }
    CPcre2CompileContext::CPcre2CompileContext(const CPcre2CompileContext& ctx)
    {
        if (!ctx.IsEmpty())
        {
            pcre2_compile_context *copyCtx = pcre2_compile_context_copy(ctx);
            if (copyCtx == nullptr)
                throw CPcre2Exception(Pcre2MakeCompileContextFail, "Fail to copy from other compile context");
            
            m_context = copyCtx;
        }
        else
            m_context = nullptr;
    }
    CPcre2CompileContext::~CPcre2CompileContext() noexcept
    {
        if (m_context != nullptr)
            pcre2_compile_context_free(m_context);
    }
    CPcre2CompileContext& CPcre2CompileContext::operator=(const CPcre2CompileContext& ctx)
    {
        if (this != &ctx)
        {
            CPcre2CompileContext tmp(ctx);
            Swap(tmp);
        }

        return *this;
    }
    void CPcre2CompileContext::Swap(CPcre2CompileContext& ctx) noexcept
    {
        std::swap(m_context, ctx.m_context);
    }

    CPcre2MatchData::CPcre2MatchData(uint32_t ovecsize, std::optional<CPcre2GeneralContext> ctx)
    {
        pcre2_match_data* md = pcre2_match_data_create(ovecsize, ctx.value_or(s_emptyGeneralCtx));
        if (md == nullptr)
            throw CPcre2Exception(Pcre2CreateMatchDataFail, "maybe invalid ovecsize");

        m_matchData = md;
    }

    CPcre2MatchData::~CPcre2MatchData() noexcept
    {
        if (m_matchData != nullptr)
            pcre2_match_data_free(m_matchData);
    }



}