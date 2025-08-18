#pragma once

#include "Graphics/Vulkan/Context.hpp"

#include <vector>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

namespace Solaris::Graphics::Vulkan {

#if defined(__APPLE__) || defined(__MACH__)
const std::vector<const char*> deviceExtensions = {vk::KHRSwapchainExtensionName, "VK_KHR_portability_subset"};
#else
const std::vector<const char*> deviceExtensions = {vk::KHRSwapchainExtensionName};
#endif

auto PickPhysicalDevice(VulkanContext&) -> void;
auto CreateLogicalDevice(VulkanContext&) -> void;

//    private:
// static auto isDeviceSuitable(const vk::raii::PhysicalDevice& device) -> bool;
// static auto rateDeviceSuitability(const vk::raii::PhysicalDevice& device, VulkanContext& ctx) -> int;
// static auto checkDeviceExtensionSupport(const vk::raii::PhysicalDevice& device) -> bool;
// };

}  // namespace Solaris::Graphics::Vulkan