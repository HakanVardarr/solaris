#pragma once

#include <GLFW/glfw3.h>
#include <spdlog/fmt/bundled/format.h>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

namespace Solaris::Graphics::Vulkan {

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

    auto init(GLFWwindow*) -> void;

   private:
    auto createInstance() -> void;
    auto checkValidationLayerSupport() -> bool;
    auto getRequiredExtensions() -> std::vector<const char*>;
};

}  // namespace Solaris::Graphics::Vulkan
