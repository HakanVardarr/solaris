#pragma once

#include "Macros.hpp"
#include "core/InstanceManager.hpp"

#include <vulkan/vulkan.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class ApplicationError {
   public:
    enum class ErrorCode : uint8_t {
        GLFWInitialization,
        WindowCreation,
        InstanceManagerError,
    };

    ApplicationError(ErrorCode errorCode) : mErrorCode(errorCode) {}
    ApplicationError(ErrorCode errorCode, const std::string& errorMessage)
        : mErrorCode(errorCode), mErrorMessage(errorMessage) {}

    [[nodiscard]] auto ToString() const -> std::string {
        std::string msg;
        switch (mErrorCode) {
            case ErrorCode::GLFWInitialization:
                msg = "Failed to initialize GLFW!";
                break;
            case ErrorCode::WindowCreation:
                msg = "Failed to create window.";
                break;
            case ErrorCode::InstanceManagerError:
                msg = mErrorMessage;
                break;
            default:
                msg = "Unknown error.";
                break;
        }

        if (mErrorCode != ErrorCode::InstanceManagerError && !mErrorMessage.empty()) {
            msg += " (" + mErrorMessage + ")";
        }

        return msg;
    }

    [[nodiscard]] auto errorCode() const -> ErrorCode { return mErrorCode; }
    [[nodiscard]] auto errorMessage() const -> const std::string& { return mErrorMessage; }

   private:
    ErrorCode mErrorCode;
    std::string mErrorMessage;
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
