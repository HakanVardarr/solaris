#include "Graphics/Vulkan/Frame.hpp"
#include <vulkan/vulkan_raii.hpp>

namespace Solaris::Graphics::Vulkan {

void Frame::init(vk::raii::Device& device) {
    vk::FenceCreateInfo f{vk::FenceCreateFlagBits::eSignaled};
    vk::SemaphoreCreateInfo s{};

    inFlightFence = device.createFence(f);
    imageAvailableSemaphore = device.createSemaphore(s);
    renderFinishedSemaphore = device.createSemaphore(s);
}

void Frames::init(vk::raii::Device& device, size_t frameCount) {
    frames.resize(frameCount);
    for (auto& frame : frames) {
        frame.init(device);
    }
}

}  // namespace Solaris::Graphics::Vulkan