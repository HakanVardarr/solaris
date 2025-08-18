#include "Graphics/Vulkan/CommandPool.hpp"
#include <vulkan/vulkan_enums.hpp>
#include "Graphics/Vulkan/QueueFamily.hpp"

namespace Solaris::Graphics::Vulkan {

auto CreateCommandPool(VulkanContext& ctx) -> void {
    auto queueFamilyIndices = FindQueueFamilies(ctx, ctx.mPhysicalDevice);

    vk::CommandPoolCreateInfo poolInfo{};
    poolInfo.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
    poolInfo.setQueueFamilyIndex(queueFamilyIndices.graphicsFamily.value());

    ctx.mCommandPool = {ctx.mDevice, poolInfo};
}

}  // namespace Solaris::Graphics::Vulkan