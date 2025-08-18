#pragma once
#include <vulkan/vulkan_raii.hpp>

namespace Solaris::Graphics::Vulkan {

struct SwapchainSupportDetails {
    vk::SurfaceCapabilitiesKHR capabilities;
    std::vector<vk::SurfaceFormatKHR> formats;
    std::vector<vk::PresentModeKHR> presentModes;
};

[[nodiscard]] SwapchainSupportDetails QuerySwapChainSupport(const vk::raii::SurfaceKHR&,
                                                            const vk::raii::PhysicalDevice&);

}  // namespace Solaris::Graphics::Vulkan