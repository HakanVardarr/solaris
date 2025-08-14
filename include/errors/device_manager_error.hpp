#pragma once

#include <cstdint>
#include <string>

namespace solaris::errors {

struct DeviceManagerError {
    enum class ErrorCode : uint8_t {
        eFailToFindGPU,
        eFailedToEnumerate,
        eFailedToCreateDevice,
        eFailedToGetQueue,
    };

    ErrorCode mErrorCode;
    std::string mErrorMessage{};

    DeviceManagerError(ErrorCode code) : mErrorCode(code){};
    DeviceManagerError(ErrorCode code, std::string message) : mErrorCode(code), mErrorMessage(message){};
};

}  // namespace solaris::errors