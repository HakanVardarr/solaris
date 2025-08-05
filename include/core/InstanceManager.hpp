#pragma once
#include "Macros.hpp"

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

#include <cstdint>

class InstanceManagerError {
   public:
    enum class ErrorCode : uint8_t {
        FailedToCreateInstance,
        EnumerateExtensions,
        RequiredExtensionDoesNotExists,
    };

    InstanceManagerError(ErrorCode errorCode, std::string errorMessage)
        : mErrorCode(errorCode), mErrorMessage(errorMessage) {}

    [[nodiscard]] auto ToString() const -> std::string {
        std::string msg;
        switch (mErrorCode) {
            case ErrorCode::FailedToCreateInstance:
                msg = "Failed to create Vulkan instance!";
                break;
            case ErrorCode::EnumerateExtensions:
                msg = "Encountered error while enumerating extensions.";
                break;
            case ErrorCode::RequiredExtensionDoesNotExists:
                msg = "Required extension does not exist.";
                break;
            default:
                msg = "Unknown error.";
                break;
        }

        if (!mErrorMessage.empty()) {
            msg += " (" + mErrorMessage + ")";
        }

        return msg;
    }

    [[nodiscard]] auto errorCode() const -> ErrorCode { return mErrorCode; }
    [[nodiscard]] auto errorMessage() const -> const std::string& { return mErrorMessage; }

   private:
    ErrorCode mErrorCode;
    std::string mErrorMessage{};
};

class InstanceManager {
   public:
    InstanceManager(){};
    ~InstanceManager() { mInstance.destroy(); };

    auto CreateInstance() -> EXPECT_VOID(InstanceManagerError);

   private:
    vk::Instance mInstance;
};
