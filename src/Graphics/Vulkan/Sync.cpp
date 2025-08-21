#include "Graphics/Vulkan/Context.hpp"

#include <vulkan/vulkan_enums.hpp>

namespace Solaris::Graphics::Vulkan {

void Context::initSync() {
    vk::FenceCreateInfo f{vk::FenceCreateFlagBits::eSignaled};
    vk::SemaphoreCreateInfo s{};

    frames.resize(swapchainImages.size());
    for (auto& frame : frames) {
        frame.inFlightFence = device.createFence(f);
        frame.imageAvailableSemaphore = device.createSemaphore(s);
        frame.renderFinishedSemaphore = device.createSemaphore(s);
    }
}

}  // namespace Solaris::Graphics::Vulkan