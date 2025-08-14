#pragma once
#include "core/vulkan_context.hpp"
#include "errors/surface_manager_error.hpp"
#include "macros.hpp"

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_raii.hpp>
#include <vulkan/vulkan_structs.hpp>

namespace solaris::core {

using solaris::errors::SurfaceManagerError;

class VulkanContext;

struct SwapchainSupportDetails {
    vk::SurfaceCapabilitiesKHR capabilities;
    std::vector<vk::SurfaceFormatKHR> formats;
    std::vector<vk::PresentModeKHR> presentModes;
};

class SurfaceManager {
   public:
    ~SurfaceManager() { mSurfaceKHR.clear(); }

    static auto Create(VulkanContext&, GLFWwindow*) -> EXPECT_VOID(SurfaceManagerError);
    static auto QuerySwapChainSupport(VulkanContext&, vk::raii::PhysicalDevice)
        -> std::expected<SwapchainSupportDetails, SurfaceManagerError>;

   private:
    vk::raii::SurfaceKHR mSurfaceKHR{nullptr};
};

}  // namespace solaris::core