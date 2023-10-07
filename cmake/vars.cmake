# see Makefile.vars

set(TIKTOKEN_PRIVATE_HEADERS
    tiktoken/include/global_define.h
    tiktoken/include/sys_env.h
    tiktoken/include/utils.h
    tiktoken/include/core_bpe.h
    tiktoken/include/error_handler.h
    tiktoken/include/model.h
    tiktoken/include/registry.h
    tiktoken/include/token_encoding.h
    tiktoken/include/tiktoken.h
)

set(TIKTOKEN_COMMON_HEADERS
    common/Utf8String.h
    common/ScopeGuard.h
    common/pcre2cpp.h
)

set(TIKTOKEN_COMMON_SRCS
    common/Utf8String.cpp
    common/pcre2cpp.cpp
)

set(TOKENTEST_COMMON_HEADERS
    common/Utf8String.h
    common/ScopeGuard.h
)

set(TOKENTEST_COMMON_SRCS
    common/Utf8String.cpp
)
