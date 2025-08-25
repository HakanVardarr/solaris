#pragma once
#include "Graphics/Vulkan/Allocator.hpp"
#include "Graphics/Vulkan/Frame.hpp"

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

#include <vk_mem_alloc.hpp>

namespace Solaris::Graphics::Vulkan {

struct Context {
    // Core
    vk::raii::Context vulkanContext{};
    vk::raii::Instance instance{nullptr};
    vk::raii::DebugUtilsMessengerEXT debugMessenger{nullptr};
    vk::raii::SurfaceKHR surface{nullptr};
    vk::raii::PhysicalDevice physicalDevice{nullptr};
    vk::raii::Device device{nullptr};
    vk::Queue graphicsQueue{nullptr};
    vk::Queue presentQueue{nullptr};
    Allocator allocator;

    // Swapchain + Swapchain resources
    vk::raii::SwapchainKHR swapchain{nullptr};
    vk::Format swapchainFormat{};
    vk::Extent2D swapchainExtent{};
    std::vector<vk::Image> swapchainImages{};
    std::vector<vk::raii::ImageView> swapchainViews{};
    std::vector<vk::raii::Framebuffer> swapchainFramebuffers{};

    // Render pass
    vk::raii::RenderPass renderPass{nullptr};

    // Command Pool
    vk::raii::CommandPool commandPool{nullptr};

    // Frames
    Frames frames;

#if defined(NDEBUG)
    bool validationEnabled = false;
#else
    bool validationEnabled = true;
#endif
    // API
    void init(GLFWwindow* window);
    void initCore(GLFWwindow* window);
    void initSwapchain(GLFWwindow* window, const vk::raii::SwapchainKHR& oldSwapchain = {nullptr});
    void initSwapchainResources();         // views, framebuffers
    void initCommands(size_t frameCount);  // command pool
    void recreateSwapchain(GLFWwindow* window);
    void destroySwapchainResources();
};

}  // namespace Solaris::Graphics::Vulkan