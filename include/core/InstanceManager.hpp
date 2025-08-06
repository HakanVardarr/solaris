#pragma once
#include "Macros.hpp"

#include <GLFW/glfw3.h>
#include <spdlog/fmt/bundled/format.h>
#include <vulkan/vulkan.hpp>

#include <cstdint>

struct InstanceManagerError {
    enum class ErrorCode : uint8_t {
        eFailedToCreateInstance,
        eEnumerateExtensions,
        eRequiredExtensionDoesNotExists,

    };

    ErrorCode mErrorCode;
    std::string mErrorMessage{};
};

template <>
struct fmt::formatter<InstanceManagerError> {
    constexpr auto parse(fmt::format_parse_context& ctx) -> const char* { return ctx.end(); }

    template <typename FormatContext>
    auto format(const InstanceManagerError& err, FormatContext& ctx) const {
        std::string_view msg;
        switch (err.mErrorCode) {
            case InstanceManagerError::ErrorCode::eFailedToCreateInstance:
                msg = "Failed to create Vulkan instance!";
                break;
            case InstanceManagerError::ErrorCode::eEnumerateExtensions:
                msg = "Encountered error while enumerating extensions.";
                break;
            case InstanceManagerError::ErrorCode::eRequiredExtensionDoesNotExists:
                msg = "Required extension does not exists";
                break;
            default:
                msg = "Unknown error.";
                break;
        }

        std::string full = std::format("{} ({})", msg, err.mErrorMessage);
        return fmt::format_to(ctx.out(), "{}", full);
    }
};

class InstanceManager {
   public:
    InstanceManager(){};
    ~InstanceManager() { mInstance.destroy(); };

    auto CreateInstance() -> EXPECT_VOID(InstanceManagerError);

   private:
    vk::Instance mInstance;
};
