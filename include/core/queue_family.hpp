#pragma once

#include <cstdint>
#include <optional>

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>
#include "core/vulkan_context.hpp"

namespace solaris::core {

class VulkanContext;

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    auto IsComplete() -> bool { return graphicsFamily.has_value() && presentFamily.has_value(); }
};

auto findQueueFamilies(const vk::raii::PhysicalDevice& device, VulkanContext& ctx) -> QueueFamilyIndices;

}  // namespace solaris::core