#include "registry.h"
#include "error_handler.h"

/*
Args:
    name: The name of the encoding. It should be clear from the name of the encoding
        what behaviour to expect, in particular, encodings with different special tokens
        should have different names.
    pat_str: A regex pattern string that is used to split the input text.
    mergeable_ranks: A dictionary mapping mergeable token bytes to their ranks. The ranks
        must correspond to merge priority.
    special_tokens: A dictionary mapping special token strings to their token values.
    explicit_n_vocab: The number of tokens in the vocabulary. If provided, it is checked
        that the number of mergeable tokens and special tokens is equal to this number.
*/

namespace TiktokenCpp
{
    const char* const ENDOFTEXT = "<|endoftext|>";
    const char* const FIM_PREFIX = "<|fim_prefix|>";
    const char* const FIM_MIDDLE = "<|fim_middle|>";
    const char* const FIM_SUFFIX = "<|fim_suffix|>";
    const char* const ENDOFPROMPT = "<|endofprompt|>";

    static EncodingParam s_encoding_param[] =
    {
        {
            "r50k_base", 50257,
            R"---('s|'t|'re|'ve|'m|'ll|'d| ?\p{L}+| ?\p{N}+| ?[^\s\p{L}\p{N}]+|\s+(?!\S)|\s+)---",
            "https://openaipublic.blob.core.windows.net/encodings/r50k_base.tiktoken",
            {{ENDOFTEXT, 50256}}
        },
        {
            "p50k_base", 50281,
            R"---('s|'t|'re|'ve|'m|'ll|'d| ?\p{L}+| ?\p{N}+| ?[^\s\p{L}\p{N}]+|\s+(?!\S)|\s+)---",
            "https://openaipublic.blob.core.windows.net/encodings/p50k_base.tiktoken",
            {{ENDOFTEXT, 50256}}
        },
        {
            "p50k_edit", std::nullopt,
            R"---('s|'t|'re|'ve|'m|'ll|'d| ?\p{L}+| ?\p{N}+| ?[^\s\p{L}\p{N}]+|\s+(?!\S)|\s+)---",
            "https://openaipublic.blob.core.windows.net/encodings/p50k_base.tiktoken",
            {{ENDOFTEXT, 50256}, {FIM_PREFIX, 50281}, {FIM_MIDDLE, 50282}, {FIM_SUFFIX, 50283}}
        },
        {
            "cl100k_base", std::nullopt,
            R"---((?i:'s|'t|'re|'ve|'m|'ll|'d)|[^\r\n\p{L}\p{N}]?\p{L}+|\p{N}{1,3}| ?[^\s\p{L}\p{N}]+[\r\n]*|\s*[\r\n]+|\s+(?!\S)|\s+)---",
            "https://openaipublic.blob.core.windows.net/encodings/cl100k_base.tiktoken",
            {{ENDOFTEXT, 100257}, {FIM_PREFIX, 100258}, {FIM_MIDDLE, 100259}, {FIM_SUFFIX, 100260}, {ENDOFPROMPT, 100276}}
        },
        {
            "error_name_for_nothing", std::nullopt,
            R"()",
            "",
            {}
        }

    };

    const EncodingParam& Registry::GetEncodingParam(const std::string_view& encoding_name)
    {
        //last one is invalid placeholder 
        int32_t count = sizeof(s_encoding_param) / sizeof(s_encoding_param[0]) - 1;
        for (int32_t i = 0; i < count; i++)
        {
            if (s_encoding_param[i].name == encoding_name)
                return s_encoding_param[i];
        }

        ThrowGeneralException("Not find encoding parameters for: ", encoding_name);

        return s_encoding_param[count - 1];
    }

}
