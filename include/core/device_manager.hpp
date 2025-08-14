#pragma once

#include "core/vulkan_context.hpp"
#include "errors/device_manager_error.hpp"
#include "macros.hpp"

#include <vector>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

#include <expected>
namespace solaris::core {

const std::vector<const char*> deviceExtensions = {vk::KHRSwapchainExtensionName};

struct DeviceManager {
    static auto pickPhysicalDevice(VulkanContext& ctx) -> EXPECT_VOID(errors::DeviceManagerError);
    static auto createLogicalDevice(VulkanContext& ctx) -> EXPECT_VOID(errors::DeviceManagerError);

   private:
    static auto isDeviceSuitable(const vk::raii::PhysicalDevice& device) -> bool;
    static auto rateDeviceSuitability(const vk::raii::PhysicalDevice& device, VulkanContext& ctx) -> int;
    static auto checkDeviceExtensionSupport(const vk::raii::PhysicalDevice& device) -> bool;
};

}  // namespace solaris::core