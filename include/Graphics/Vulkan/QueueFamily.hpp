#pragma once

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

#include <cstdint>
#include <optional>

namespace Solaris::Graphics::Vulkan {

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    auto isComplete() -> bool { return graphicsFamily.has_value() && presentFamily.has_value(); }
};

auto FindQueueFamilies(const vk::raii::PhysicalDevice& device, const vk::SurfaceKHR) -> QueueFamilyIndices;

}  // namespace Solaris::Graphics::Vulkan