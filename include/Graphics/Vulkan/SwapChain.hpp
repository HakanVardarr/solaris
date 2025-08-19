#pragma once

#include <GLFW/glfw3.h>
#include <vulkan/vulkan_raii.hpp>

namespace Solaris::Graphics::Vulkan {

struct SwapchainSupportDetails {
    vk::SurfaceCapabilitiesKHR capabilities;
    std::vector<vk::SurfaceFormatKHR> formats;
    std::vector<vk::PresentModeKHR> presentModes;
};

[[nodiscard]] SwapchainSupportDetails QuerySwapChainSupport(const vk::raii::SurfaceKHR&,
                                                            const vk::raii::PhysicalDevice&);

vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);
vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes);
vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities, GLFWwindow* window);

}  // namespace Solaris::Graphics::Vulkan