#pragma once

#include <GLFW/glfw3.h>
#include <spdlog/fmt/bundled/format.h>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>
#include <vulkan/vulkan_structs.hpp>

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
    vk::raii::SwapchainKHR mSwapChain{nullptr};
    std::vector<vk::Image> mSwapChainImages{};
    vk::Format mSwapChainImageFormat;
    vk::Extent2D mSwapChainExtent;
    std::vector<vk::raii::ImageView> mSwapChainImageViews{};
    vk::raii::RenderPass mRenderPass{nullptr};
    vk::raii::PipelineLayout mPipelineLayout{nullptr};
    vk::raii::Pipeline mPipeline{nullptr};
    std::vector<vk::raii::Framebuffer> mSwapChainFramebuffers{};
    vk::raii::CommandPool mCommandPool{nullptr};
    vk::raii::CommandBuffer mCommandBuffer{nullptr};
    vk::raii::Semaphore mImageAvailableSemaphore{nullptr};
    vk::raii::Semaphore mRenderFinishedSemaphore{nullptr};
    vk::raii::Fence mInFlightFence{nullptr};

#ifdef NDEBUG
    bool validationEnabled = false;
#else
    bool validationEnabled = true;
#endif

    auto Init(GLFWwindow*) -> void;

   private:
    auto createInstance() -> void;
    auto checkValidationLayerSupport() -> bool;
    auto getRequiredExtensions() -> std::vector<const char*>;
};

}  // namespace Solaris::Graphics::Vulkan
