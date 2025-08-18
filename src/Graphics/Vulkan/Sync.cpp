#include "Graphics/Vulkan/Sync.hpp"
#include <vulkan/vulkan_enums.hpp>

namespace Solaris::Graphics::Vulkan {

auto CreateSyncObjects(VulkanContext& ctx) -> void {
    vk::SemaphoreCreateInfo semaphoreInfo{};
    vk::FenceCreateInfo fenceInfo{};
    fenceInfo.setFlags(vk::FenceCreateFlagBits::eSignaled);

    ctx.mImageAvailableSemaphore = ctx.mDevice.createSemaphore(semaphoreInfo);
    ctx.mRenderFinishedSemaphore = ctx.mDevice.createSemaphore(semaphoreInfo);
    ctx.mInFlightFence = ctx.mDevice.createFence(fenceInfo);
}

}  // namespace Solaris::Graphics::Vulkan