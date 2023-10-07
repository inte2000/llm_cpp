#pragma once

#include <string_view>
#include <stdexcept>

namespace TiktokenCpp 
{
    void ThrowGeneralException(const std::string& reason, const std::string_view& param);

    class UnicodeEncoderException : public std::runtime_error
    {
    public:
        /*
        [[nodiscard]]
        char const* what() const override
        {
            return std::format("Pcre2 error: {0}, error code: {1}, error offset: {2}",
                std::exception::what(), m_errCode, m_errOffset).c_str();
        }
        */
        //UnicodeEncoderException()
    };

}