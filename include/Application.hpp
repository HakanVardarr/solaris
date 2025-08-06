#pragma once
#include "Macros.hpp"
#include "core/InstanceManager.hpp"

#include <spdlog/fmt/bundled/base.h>
#include <spdlog/fmt/bundled/format.h>
#include <vulkan/vulkan.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

struct ApplicationError {
    enum class ErrorCode : uint8_t {
        eGLFWInitialization,
        eWindowCreation,
        eInstanceManagerError,
    };

    ErrorCode mErrorCode;
    std::string mErrorMessage{};
};

template <>
struct fmt::formatter<ApplicationError> {
    constexpr auto parse(fmt::format_parse_context& ctx) -> const char* { return ctx.end(); }

    template <typename FormatContext>
    auto format(const ApplicationError& err, FormatContext& ctx) const {
        std::string_view msg;
        switch (err.mErrorCode) {
            case ApplicationError::ErrorCode::eGLFWInitialization:
                msg = "Failed to initalize GLFW!";
                break;
            case ApplicationError::ErrorCode::eWindowCreation:
                msg = "Failed to create window.";
                break;
            case ApplicationError::ErrorCode::eInstanceManagerError:
                msg = err.mErrorMessage;
                break;
            default:
                msg = "Unknown error.";
                break;
        }

        return fmt::format_to(ctx.out(), "{}", msg);
    }
};

class Application {
   public:
    auto Run() -> EXPECT_VOID(ApplicationError);
    ~Application();

   private:
    auto initWindow() -> EXPECT_VOID(ApplicationError);
    auto initVulkan() -> EXPECT_VOID(ApplicationError);
    auto mainLoop() -> EXPECT_VOID(ApplicationError);

    GLFWwindow* pWindow = nullptr;
    InstanceManager mInstanceManager;
};
