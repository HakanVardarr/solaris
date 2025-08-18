#include "Graphics/Vulkan/Context.hpp"

#include <vulkan/vulkan_enums.hpp>

namespace Solaris::Graphics::Vulkan {

void Context::initSync() {
    vk::FenceCreateInfo f{vk::FenceCreateFlagBits::eSignaled};
    vk::SemaphoreCreateInfo s{};

    inFlightFence = device.createFence(f);
    imageAvailableSemaphore = device.createSemaphore(s);
    renderFinishedSemaphore = device.createSemaphore(s);
}

}  // namespace Solaris::Graphics::Vulkan