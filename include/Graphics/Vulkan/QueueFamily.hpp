#pragma once
#include "Graphics/Vulkan/Context.hpp"

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

#include <cstdint>
#include <optional>

namespace Solaris::Graphics::Vulkan {

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    auto IsComplete() -> bool { return graphicsFamily.has_value() && presentFamily.has_value(); }
};

auto findQueueFamilies(const vk::raii::PhysicalDevice& device, VulkanContext& ctx) -> QueueFamilyIndices;

}  // namespace Solaris::Graphics::Vulkan