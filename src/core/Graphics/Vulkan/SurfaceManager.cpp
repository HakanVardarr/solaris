#include "Graphics/Vulkan/SurfaceManager.hpp"
#include "Graphics/Vulkan/Context.hpp"

#include <GLFW/glfw3.h>
#include <vulkan/vk_enum_string_helper.h>
#include <vulkan/vulkan_core.h>

#include <stdexcept>
#include <vulkan/vulkan_raii.hpp>

namespace Solaris::Graphics::Vulkan {

auto SurfaceManager::Create(VulkanContext& ctx, GLFWwindow* pWindow) -> void {
    VkSurfaceKHR _surface;
    if (auto err = glfwCreateWindowSurface(*ctx.mInstance, pWindow, nullptr, &_surface); err != VK_SUCCESS) {
        throw std::runtime_error("Failed to create window surface.");
    }
    auto surface = vk::raii::SurfaceKHR(ctx.mInstance, _surface);
    ctx.mSurface.swap(surface);
}

auto SurfaceManager::QuerySwapChainSupport(VulkanContext& ctx, vk::raii::PhysicalDevice device)
    -> SwapchainSupportDetails {
    SwapchainSupportDetails details;

    details.formats = device.getSurfaceFormatsKHR(ctx.mSurface);
    details.capabilities = device.getSurfaceCapabilitiesKHR(ctx.mSurface);
    details.presentModes = device.getSurfacePresentModesKHR(ctx.mSurface);

    return details;
}

}  // namespace Solaris::Graphics::Vulkan