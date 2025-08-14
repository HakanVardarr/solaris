#include "core/surface_manager.hpp"
#include "core/vulkan_context.hpp"
#include "errors/surface_manager_error.hpp"
#include "macros.hpp"

#include <GLFW/glfw3.h>
#include <vulkan/vk_enum_string_helper.h>
#include <vulkan/vulkan_core.h>

#include <vulkan/vulkan_raii.hpp>

namespace solaris::core {

auto SurfaceManager::Create(VulkanContext& ctx, GLFWwindow* pWindow) -> EXPECT_VOID(SurfaceManagerError) {
    VkSurfaceKHR _surface;
    if (auto err = glfwCreateWindowSurface(*ctx.mInstance, pWindow, nullptr, &_surface); err != VK_SUCCESS) {
        std::unexpected(
            SurfaceManagerError(SurfaceManagerError::ErrorCode::eFailedToCreateSurface, string_VkResult(err)));
    }
    auto surface = vk::raii::SurfaceKHR(ctx.mInstance, _surface);

    ctx.mSurface.swap(surface);
    return {};
}

auto SurfaceManager::QuerySwapChainSupport(VulkanContext& ctx, vk::raii::PhysicalDevice device)
    -> std::expected<SwapchainSupportDetails, SurfaceManagerError> {
    SwapchainSupportDetails details;

    details.formats = device.getSurfaceFormatsKHR(ctx.mSurface);
    details.capabilities = device.getSurfaceCapabilitiesKHR(ctx.mSurface);
    details.presentModes = device.getSurfacePresentModesKHR(ctx.mSurface);

    return details;
}

}  // namespace solaris::core