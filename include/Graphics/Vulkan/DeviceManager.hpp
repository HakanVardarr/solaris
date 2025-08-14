#pragma once

#include "Graphics/Vulkan/Context.hpp"

#include <vector>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

namespace Solaris::Graphics::Vulkan {

const std::vector<const char*> deviceExtensions = {vk::KHRSwapchainExtensionName};

struct DeviceManager {
    static auto pickPhysicalDevice(VulkanContext& ctx) -> void;
    static auto createLogicalDevice(VulkanContext& ctx) -> void;

   private:
    static auto isDeviceSuitable(const vk::raii::PhysicalDevice& device) -> bool;
    static auto rateDeviceSuitability(const vk::raii::PhysicalDevice& device, VulkanContext& ctx) -> int;
    static auto checkDeviceExtensionSupport(const vk::raii::PhysicalDevice& device) -> bool;
};

}  // namespace Solaris::Graphics::Vulkan