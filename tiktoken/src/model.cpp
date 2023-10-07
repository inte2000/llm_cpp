#include <unordered_map>
#include <string>
#include <boost/algorithm/string.hpp>
#include "model.h"
#include "error_handler.h"


namespace TiktokenCpp
{
    using ModelNameHash = std::unordered_map<std::string_view, std::string_view>;
    
    static ModelNameHash MODEL_PREFIX_TO_ENCODING = 
    {
        //chat
        {"gpt-4-", "cl100k_base"},  // e.g., gpt-4-0314, etc., plus gpt-4-32k
        {"gpt-3.5-turbo-", "cl100k_base"},  // e.g, gpt-3.5-turbo-0301, -0401, etc.
        {"gpt-35-turbo-", "cl100k_base"},  // Azure deployment name
        //fine-tuned
        {"ft:gpt-4", "cl100k_base"},
        {"ft:gpt-3.5-turbo", "cl100k_base"},
        {"ft:davinci-002", "cl100k_base"},
        {"ft:babbage-002", "cl100k_base"}
    };

    static ModelNameHash MODEL_TO_ENCODING =
    {
        //chat
        {"gpt-4", "cl100k_base"},
        {"gpt-3.5-turbo", "cl100k_base"},
        {"gpt-35-turbo", "cl100k_base"},  //Azure deployment name
        //base
        {"davinci-002", "cl100k_base"},
        {"babbage-002", "cl100k_base"},
        //embeddings
        {"text-embedding-ada-002", "cl100k_base"},
        //DEPRECATED MODELS
        //text(DEPRECATED)
        {"text-davinci-003", "p50k_base"},
        {"text-davinci-002", "p50k_base"},
        {"text-davinci-001", "r50k_base"},
        {"text-curie-001", "r50k_base"},
        {"text-babbage-001", "r50k_base"},
        {"text-ada-001", "r50k_base"},
        {"davinci", "r50k_base"},
        {"curie", "r50k_base"},
        {"babbage", "r50k_base"},
        {"ada", "r50k_base"},
        //code(DEPRECATED)
        {"code-davinci-002", "p50k_base"},
        {"code-davinci-001", "p50k_base"},
        {"code-cushman-002", "p50k_base"},
        {"code-cushman-001", "p50k_base"},
        {"davinci-codex", "p50k_base"},
        {"cushman-codex", "p50k_base"},
        //edit(DEPRECATED)
        {"text-davinci-edit-001", "p50k_edit"},
        {"code-davinci-edit-001", "p50k_edit"},
        //old embeddings(DEPRECATED)
        {"text-similarity-davinci-001", "r50k_base"},
        {"text-similarity-curie-001", "r50k_base"},
        {"text-similarity-babbage-001", "r50k_base"},
        {"text-similarity-ada-001", "r50k_base"},
        {"text-search-davinci-doc-001", "r50k_base"},
        {"text-search-curie-doc-001", "r50k_base"},
        {"text-search-babbage-doc-001", "r50k_base"},
        {"text-search-ada-doc-001", "r50k_base"},
        {"code-search-babbage-code-001", "r50k_base"},
        {"code-search-ada-code-001", "r50k_base"},
        //open source
        {"gpt2", "gpt2"}
    };

    std::string_view Model::EncodingNameForModel(const std::string_view& model_name)
    {
        std::string_view encoding_name;
        auto it = MODEL_TO_ENCODING.find(model_name);
        if (it != MODEL_TO_ENCODING.end())
        {
            encoding_name = it->second;
        }
        else
        {
            auto it = std::find_if(MODEL_PREFIX_TO_ENCODING.begin(), MODEL_PREFIX_TO_ENCODING.end(),
                [&model_name](const auto& item)
                { return boost::starts_with(model_name, item.first); });
            if (it != MODEL_PREFIX_TO_ENCODING.end())
            {
                encoding_name = it->second;
            }
        }

        if (encoding_name.empty())
        {
            ThrowGeneralException("not find encoding name for model: ", model_name);
        }

        return encoding_name;
    }

}
