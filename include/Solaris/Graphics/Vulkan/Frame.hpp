#pragma once

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_raii.hpp>

namespace Solaris::Graphics::Vulkan {

struct Frame {
    vk::raii::CommandBuffer commandBuffer{nullptr};
    vk::raii::Semaphore imageAvailableSemaphore{nullptr};
    vk::raii::Semaphore renderFinishedSemaphore{nullptr};
    vk::raii::Fence inFlightFence{nullptr};

    void init(vk::raii::Device& device);
};

class Frames {
   public:
    void init(vk::raii::Device& device, size_t frameCount);

    [[nodiscard]] Frame& getCurrentFrame() { return frames[currentFrame]; }
    void updateFrame() { currentFrame = (currentFrame + 1) % frames.size(); }
    [[nodiscard]] std::vector<Frame>& getAll() { return frames; }

   private:
    uint32_t currentFrame = 0;
    std::vector<Frame> frames;
};

}  // namespace Solaris::Graphics::Vulkan