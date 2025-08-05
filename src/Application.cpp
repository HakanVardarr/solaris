#include "Application.hpp"

#include <spdlog/spdlog.h>

#include <expected>
#include <format>

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
        return std::unexpected(ApplicationError(ApplicationError::ErrorCode::GLFWInitialization));
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    pWindow = glfwCreateWindow(WIDTH, HEIGHT, "solaris", nullptr, nullptr);
    if (pWindow == nullptr) {
        return std::unexpected(ApplicationError(ApplicationError::ErrorCode::InstanceManagerError));
    }

    return {};
}

auto Application::initVulkan() -> EXPECT_VOID(ApplicationError) {
    if (auto result = mInstanceManager.CreateInstance(); !result) {
        return std::unexpected(ApplicationError(ApplicationError::ErrorCode::InstanceManagerError,
                                                std::format("{}", result.error().ToString())));
    }

    return {};
}
auto Application::mainLoop() -> EXPECT_VOID(ApplicationError) {
    while (glfwWindowShouldClose(pWindow) == GLFW_FALSE) {
        glfwPollEvents();
    }

    return {};
}
