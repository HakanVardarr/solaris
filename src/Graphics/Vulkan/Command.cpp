#include "Graphics/Vulkan/Context.hpp"

#include <vulkan/vulkan_enums.hpp>

namespace Solaris::Graphics::Vulkan {

void Context::initCommands() {
    // Command Pool
    vk::CommandPoolCreateInfo c{vk::CommandPoolCreateFlagBits::eResetCommandBuffer, 0};
    commandPool = {device, c};

    // Command Buffer
    vk::CommandBufferAllocateInfo ci{};
    ci.setCommandPool(commandPool);
    ci.setLevel(vk::CommandBufferLevel::ePrimary);
    ci.setCommandBufferCount(1);

    auto buffers = device.allocateCommandBuffers(ci);
    commandBuffer = std::move(buffers[0]);
}

}  // namespace Solaris::Graphics::Vulkan