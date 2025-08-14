#pragma once

#include <cstdint>
#include <string>

namespace solaris::errors {

struct VulkanContextError {
    enum class ErrorCode : uint8_t {
        eEnumerateInstanceProperties,
        eInstanceCreate,
        eDebugMessenger,
        eDeviceManager,
        eSurfaceManager,
    };

    ErrorCode mErrorCode;
    std::string mErrorMessage{};

    VulkanContextError(ErrorCode code) : mErrorCode(code){};
    VulkanContextError(ErrorCode code, std::string message) : mErrorCode(code), mErrorMessage(message){};
};

}  // namespace solaris::errors