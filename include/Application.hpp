#pragma once

#include "../include/Macros.hpp"
#include "core/InstanceManager.hpp"

#include <vulkan/vulkan.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <format>
#include <string_view>

struct ApplicationError {
    enum class ErrorCode : uint8_t {
        GLFWInitialization,
        WindowCreation,
        InstanceManagerError,
    };

    ErrorCode errorCode;
    std::string errorMessage{};
};

template <>
struct std::formatter<ApplicationError> : std::formatter<std::string_view> {
    auto format(ApplicationError err, format_context& ctx) const {
        std::string_view msg;
        switch (err.errorCode) {
            case ApplicationError::ErrorCode::GLFWInitialization:
                msg = "Failed to initialize GLFW!";
                break;
            case ApplicationError::ErrorCode::WindowCreation:
                msg = "Failed to create window.";
                break;
            case ApplicationError::ErrorCode::InstanceManagerError:
                msg = err.errorMessage;
                break;
            default:
                msg = "Unknown error.";
                break;
        }
        return std::formatter<std::string_view>::format(msg, ctx);
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
