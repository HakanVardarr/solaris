#pragma once
#include "Graphics/Vulkan/Context.hpp"

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_raii.hpp>
#include <vulkan/vulkan_structs.hpp>

namespace Solaris::Graphics::Vulkan {

auto CreateSurface(VulkanContext&, GLFWwindow*) -> void;

}  // namespace Solaris::Graphics::Vulkan