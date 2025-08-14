#pragma once

#include <cstdint>
#include <string>

namespace solaris::errors {

struct SurfaceManagerError {
    enum class ErrorCode : uint8_t { eFailedToCreateSurface };

    ErrorCode mErrorCode;
    std::string mErrorMessage{};

    SurfaceManagerError(ErrorCode code) : mErrorCode(code){};
    SurfaceManagerError(ErrorCode code, std::string message) : mErrorCode(code), mErrorMessage(message){};
};

}  // namespace solaris::errors