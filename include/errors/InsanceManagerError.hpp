#pragma once

#include <cstdint>
#include <string>

namespace solaris::errors {

struct InstanceManagerError {
    enum class ErrorCode : uint8_t {
        eFailedToCreateInstance,
        eEnumerateExtensions,
        eInstanceLayerProperties,
        eRequiredExtensionDoesNotExists,
        eValidationLayersNotAvailable,

    };

    ErrorCode mErrorCode;
    std::string mErrorMessage{};
};

}  // namespace solaris::errors