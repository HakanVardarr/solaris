#include "Graphics/Vulkan/Context.hpp"
#include "Graphics/Vulkan/Vertex.hpp"

#include <spdlog/spdlog.h>
#include <vulkan/vulkan_enums.hpp>

#include <utility>

namespace Solaris::Graphics::Vulkan {

void Context::initCommands(size_t frameCount) {
    // Frames
    frames.getAll().resize(frameCount);

    // Command Pool
    vk::CommandPoolCreateInfo c{vk::CommandPoolCreateFlagBits::eResetCommandBuffer, 0};
    commandPool = {device, c};

    // Command Buffer
    vk::CommandBufferAllocateInfo ci{};
    ci.setCommandPool(commandPool);
    ci.setLevel(vk::CommandBufferLevel::ePrimary);
    ci.setCommandBufferCount(swapchainImages.size());

    auto buffers = device.allocateCommandBuffers(ci);
    size_t i = 0;
    for (auto& frame : frames.getAll()) {
        frame.commandBuffer = std::move(buffers[i++]);
    }

    frames.init(device, frameCount);
    spdlog::info("Command pool created. Allocated {} command buffers.", buffers.size());
}

}  // namespace Solaris::Graphics::Vulkan