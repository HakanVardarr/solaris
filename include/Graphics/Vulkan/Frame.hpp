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
};

}  // namespace Solaris::Graphics::Vulkan