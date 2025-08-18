#include "Graphics/Vulkan/QueueFamily.hpp"
#include <vulkan/vulkan_handles.hpp>

namespace Solaris::Graphics::Vulkan {

auto FindQueueFamilies(const vk::raii::PhysicalDevice& device, const vk::SurfaceKHR surface) -> QueueFamilyIndices {
    QueueFamilyIndices indices;
    auto queueFamilies = device.getQueueFamilyProperties();
    int i = 0;
    for (const auto& queueFamily : queueFamilies) {
        if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) {
            indices.graphicsFamily = i;
        }

        auto presentSupport = device.getSurfaceSupportKHR(i, surface);
        if (presentSupport) {
            indices.presentFamily = i;
        }

        if (indices.isComplete()) {
            break;
        }

        i++;
    }
    return indices;
}

}  // namespace Solaris::Graphics::Vulkan