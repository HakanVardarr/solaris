#include "Graphics/Vulkan/Context.hpp"

#include <spdlog/spdlog.h>
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
    ci.setCommandBufferCount(MAX_FRAMES_IN_FLIGHT);

    auto buffers = device.allocateCommandBuffers(ci);
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        frames[i].commandBuffer = std::move(buffers[i]);
    }

    spdlog::info("Command pool created. Allocated {} command buffers.", buffers.size());
}

}  // namespace Solaris::Graphics::Vulkan