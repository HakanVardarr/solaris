#pragma once
#include "errors/vulkan_context_error.hpp"
#include "macros.hpp"

#include <GLFW/glfw3.h>
#include <spdlog/fmt/bundled/format.h>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

#include <expected>

namespace solaris::core {

const std::vector<const char*> validationLayers = {"VK_LAYER_KHRONOS_validation"};

struct VulkanContext {
    ~VulkanContext() {}

    vk::raii::Context mCtx{};
    vk::raii::Instance mInstance{nullptr};
    vk::raii::SurfaceKHR mSurface{nullptr};
    vk::raii::DebugUtilsMessengerEXT mDebugMessenger{nullptr};
    vk::raii::PhysicalDevice mPhysicalDevice{nullptr};
    vk::raii::Device mDevice{nullptr};
    vk::raii::Queue mGraphicsQueue{nullptr};
    vk::raii::Queue mPresentQueue{nullptr};

#ifdef NDEBUG
    bool validationEnabled = false;
#else
    bool validationEnabled = true;
#endif

    auto init(GLFWwindow*) -> EXPECT_VOID(errors::VulkanContextError);

   private:
    auto createInstance() -> EXPECT_VOID(errors::VulkanContextError);
    auto checkValidationLayerSupport() -> std::expected<bool, errors::VulkanContextError>;
    auto getRequiredExtensions() -> std::vector<const char*>;
};

}  // namespace solaris::core
