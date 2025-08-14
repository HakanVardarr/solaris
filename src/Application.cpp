#include "Application.hpp"

#include <spdlog/fmt/bundled/format.h>
#include <spdlog/spdlog.h>
#include <vulkan/vk_enum_string_helper.h>
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_raii.hpp>

#include <stdexcept>

auto Application::Run() -> void {
    initWindow();
    initVulkan();
    mainLoop();
}

Application::~Application() {
    if (pWindow != nullptr) {
        glfwDestroyWindow(pWindow);
    }

    glfwTerminate();
}

auto Application::initWindow() -> void {
    const uint32_t WIDTH = 800;
    const uint32_t HEIGHT = 600;

    if (glfwInit() == GLFW_FALSE) {
        throw std::runtime_error("Failed to initialize GLFW");
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    pWindow = glfwCreateWindow(WIDTH, HEIGHT, "solaris", nullptr, nullptr);
    if (pWindow == nullptr) {
        throw std::runtime_error("Failed to create window");
    }
}

auto Application::initVulkan() -> void {
    try {
        mContext.init(pWindow);
    } catch (vk::SystemError& err) {
        throw std::runtime_error(err.what());
    }
}
auto Application::mainLoop() -> void {
    while (glfwWindowShouldClose(pWindow) == GLFW_FALSE) {
        glfwPollEvents();
    }
}
