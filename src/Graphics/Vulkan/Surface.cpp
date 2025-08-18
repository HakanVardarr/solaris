#include "Graphics/Vulkan/Context.hpp"
#include "Graphics/Vulkan/Surface.hpp"

#include <GLFW/glfw3.h>
#include <vulkan/vk_enum_string_helper.h>
#include <vulkan/vulkan_core.h>

#include <stdexcept>
#include <vulkan/vulkan_raii.hpp>

namespace Solaris::Graphics::Vulkan {

auto CreateSurface(VulkanContext& ctx, GLFWwindow* pWindow) -> void {
    VkSurfaceKHR _surface;
    if (auto err = glfwCreateWindowSurface(*ctx.mInstance, pWindow, nullptr, &_surface); err != VK_SUCCESS) {
        throw std::runtime_error("Failed to create window surface.");
    }
    auto surface = vk::raii::SurfaceKHR(ctx.mInstance, _surface);
    ctx.mSurface.swap(surface);
}

}  // namespace Solaris::Graphics::Vulkan