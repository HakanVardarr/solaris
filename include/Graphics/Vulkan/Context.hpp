#pragma once

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

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

    // Swapchain + Swapchain resources
    vk::raii::SwapchainKHR swapchain{nullptr};
    vk::Format swapchainFormat{};
    vk::Extent2D swapchainExtent{};
    std::vector<vk::Image> swapchainImages{};
    std::vector<vk::raii::ImageView> swapchainViews{};
    std::vector<vk::raii::Framebuffer> swapchainFramebuffers{};

    // Render pass / Pipeline
    vk::raii::RenderPass renderPass{nullptr};
    vk::raii::PipelineLayout pipelineLayout{nullptr};
    vk::raii::Pipeline pipeline{nullptr};

    // Command Pool + Command Buffer
    vk::raii::CommandPool commandPool{nullptr};
    vk::raii::CommandBuffer commandBuffer{nullptr};

    // Sync
    vk::raii::Semaphore imageAvailableSemaphore{nullptr};
    vk::raii::Semaphore renderFinishedSemaphore{nullptr};
    vk::raii::Fence inFlightFence{nullptr};

#if defined(NDEBUG)
    bool validationEnabled = false;
#else
    bool validationEnabled = true;
#endif

    // API
    void init(GLFWwindow* window);
    void initCore(GLFWwindow* window);
    void initSwapchain(GLFWwindow* window);
    void initSwapchainResources();  // views, framebuffers
    void initPipeline();            // renderPass + pipeline + layout
    void initCommands();            // command pool
    void initSync();                // fence/semaphore
};

}  // namespace Solaris::Graphics::Vulkan