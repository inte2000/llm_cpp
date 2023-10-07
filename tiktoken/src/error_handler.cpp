#include <string>
#include "error_handler.h"

namespace TiktokenCpp
{
    void ThrowGeneralException(const std::string& reason, const std::string_view& param)
    {
        std::string errstr = reason;
        errstr += param;

        throw std::runtime_error(errstr.c_str());
    }
}
