#pragma once
#include "Graphics/Vulkan/Context.hpp"

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

namespace Solaris::Graphics::Vulkan {

struct SwapchainSupportDetails {
    vk::SurfaceCapabilitiesKHR capabilities;
    std::vector<vk::SurfaceFormatKHR> formats;
    std::vector<vk::PresentModeKHR> presentModes;
};

auto QuerySwapChainSupport(VulkanContext&, vk::raii::PhysicalDevice) -> SwapchainSupportDetails;
auto CreateSwapChain(VulkanContext&, GLFWwindow*) -> void;
auto CreateFrameBuffers(VulkanContext&) -> void;

}  // namespace Solaris::Graphics::Vulkan