#pragma once

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

#include <string>
#include <vector>

namespace Solaris::Graphics::Vulkan {

std::vector<char> readFile(const std::string& fileName);
vk::raii::ShaderModule createShaderModule(const vk::raii::Device& device, const std::vector<char>& code);

}  // namespace Solaris::Graphics::Vulkan