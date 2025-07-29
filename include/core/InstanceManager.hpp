#pragma once
#include "Macros.hpp"

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

#include <cstdint>

struct InstanceManagerError {
    enum class ErrorCode : uint8_t {
        FailedToCreateInstance,
        EnumerateExtensions,
        RequiredExtensionDoesNotExists,
    };

    ErrorCode errorCode;
    std::string errorMessage{};
};

template <>
struct std::formatter<InstanceManagerError> : std::formatter<std::string_view> {
    auto format(InstanceManagerError err, format_context& ctx) const {
        std::string_view msg;
        switch (err.errorCode) {
            case InstanceManagerError::ErrorCode::FailedToCreateInstance:
                msg = "Failed to create Vulkan instance!";
                break;
            case InstanceManagerError::ErrorCode::EnumerateExtensions:
                msg = "Encountered error while enumerating extensions.";
                break;
            case InstanceManagerError::ErrorCode::RequiredExtensionDoesNotExists:
                msg = "Required extension does not exists";
                break;
            default:
                msg = "Unknown error.";
                break;
        }
        std::string full = std::format("{} ({})", msg, err.errorMessage);
        return std::formatter<std::string_view>::format(full, ctx);
    }
};

class InstanceManager {
   public:
    InstanceManager() {};
    ~InstanceManager() { mInstance.destroy(); };

    auto CreateInstance() -> EXPECT_VOID(InstanceManagerError);

   private:
    vk::Instance mInstance;
};
