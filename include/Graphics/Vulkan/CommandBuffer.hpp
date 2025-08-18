#pragma once
#include "Graphics/Vulkan/Context.hpp"

namespace Solaris::Graphics::Vulkan {

auto CreateCommandBuffer(VulkanContext&) -> void;

}  // namespace Solaris::Graphics::Vulkan