#pragma once

#include <cstdint>
#include <string>

namespace solaris::errors {

struct ApplicationError {
    enum class ErrorCode : uint8_t {
        eGLFWInitialization,
        eWindowCreation,
        eVulkanContext,
    };

    ErrorCode mErrorCode;
    std::string mErrorMessage{};
};

}  // namespace solaris::errors