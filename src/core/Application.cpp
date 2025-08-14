#include "core/application.hpp"
#include "errors/application_error.hpp"
#include "fmt/formatters.hpp"  // IWYU pragma: keep
#include "macros.hpp"

#include <spdlog/fmt/bundled/format.h>
#include <spdlog/spdlog.h>
#include <vulkan/vulkan_core.h>

#include <vulkan/vk_enum_string_helper.h>
#include <expected>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_raii.hpp>

namespace solaris::core {

using solaris::errors::ApplicationError;

auto Application::Run() -> EXPECT_VOID(ApplicationError) {
    RETURN_ERROR(initWindow());
    RETURN_ERROR(initVulkan());
    RETURN_ERROR(mainLoop());

    return {};
}

Application::~Application() {
    if (pWindow != nullptr) {
        glfwDestroyWindow(pWindow);
    }

    glfwTerminate();
}

auto Application::initWindow() -> EXPECT_VOID(ApplicationError) {
    const uint32_t WIDTH = 800;
    const uint32_t HEIGHT = 600;

    if (glfwInit() == GLFW_FALSE) {
        return std::unexpected(ApplicationError{.mErrorCode = ApplicationError::ErrorCode::eGLFWInitialization});
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    pWindow = glfwCreateWindow(WIDTH, HEIGHT, "solaris", nullptr, nullptr);
    if (pWindow == nullptr) {
        return std::unexpected(ApplicationError{.mErrorCode = ApplicationError::ErrorCode::eWindowCreation});
    }

    return {};
}

auto Application::initVulkan() -> EXPECT_VOID(ApplicationError) {
    if (auto result = mContext.init(pWindow); !result) {
        return std::unexpected(ApplicationError{.mErrorCode = ApplicationError::ErrorCode::eVulkanContext,
                                                .mErrorMessage = fmt::format("{}", result.error())});
    }

    return {};
}
auto Application::mainLoop() -> EXPECT_VOID(ApplicationError) {
    while (glfwWindowShouldClose(pWindow) == GLFW_FALSE) {
        glfwPollEvents();
    }

    return {};
}

}  // namespace solaris::core
