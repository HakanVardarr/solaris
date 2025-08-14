#include "Graphics/Vulkan/QueueFamily.hpp"
#include "Graphics/Vulkan/Context.hpp"

namespace Solaris::Graphics::Vulkan {

auto findQueueFamilies(const vk::raii::PhysicalDevice& device, VulkanContext& ctx) -> QueueFamilyIndices {
    QueueFamilyIndices indices;

    auto queueFamilies = device.getQueueFamilyProperties();
    int i = 0;
    for (const auto& queueFamily : queueFamilies) {
        if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) {
            indices.graphicsFamily = i;
        }

        auto presentSupport = device.getSurfaceSupportKHR(i, ctx.mSurface);
        if (presentSupport) {
            indices.presentFamily = i;
        }

        if (indices.IsComplete()) {
            break;
        }

        i++;
    }

    return indices;
}

}  // namespace Solaris::Graphics::Vulkan