#pragma once
#include "Graphics/Vulkan/Context.hpp"

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_raii.hpp>
#include <vulkan/vulkan_structs.hpp>

namespace Solaris::Graphics::Vulkan {

struct SwapchainSupportDetails {
    vk::SurfaceCapabilitiesKHR capabilities;
    std::vector<vk::SurfaceFormatKHR> formats;
    std::vector<vk::PresentModeKHR> presentModes;
};

class SurfaceManager {
   public:
    ~SurfaceManager() { mSurfaceKHR.clear(); }

    static auto Create(VulkanContext&, GLFWwindow*) -> void;
    static auto QuerySwapChainSupport(VulkanContext&, vk::raii::PhysicalDevice) -> SwapchainSupportDetails;

   private:
    vk::raii::SurfaceKHR mSurfaceKHR{nullptr};
};

}  // namespace Solaris::Graphics::Vulkan