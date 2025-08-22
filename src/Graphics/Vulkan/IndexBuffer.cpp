// #include "Graphics/Vulkan/Allocator.hpp"
// #include "Graphics/Vulkan/Context.hpp"
// #include "Graphics/Vulkan/Vertex.hpp"
// #include "Graphics/Vulkan/VmaBuffer.hpp"

// #include <vk_mem_alloc_enums.hpp>
// #include <vk_mem_alloc_handles.hpp>
// #include <vk_mem_alloc_structs.hpp>
// #include <vulkan/vulkan_enums.hpp>
// #include <vulkan/vulkan_raii.hpp>
// #include <vulkan/vulkan_structs.hpp>

// #include <cstring>

// namespace Solaris::Graphics::Vulkan {

// void Context::initIndexBuffer() {
//     vk::BufferCreateInfo bufferInfo{};
//     bufferInfo.setSize(sizeof(indices[0]) * indices.size());
//     bufferInfo.setUsage(vk::BufferUsageFlagBits::eIndexBuffer);
//     bufferInfo.setSharingMode(vk::SharingMode::eExclusive);

//     vma::AllocationCreateInfo allocCreateInfo{};
//     allocCreateInfo.setUsage(vma::MemoryUsage::eAuto);
//     allocCreateInfo.setFlags(vma::AllocationCreateFlagBits::eHostAccessSequentialWrite |
//                              vma::AllocationCreateFlagBits::eMapped);

//     vertexBuffer = {&*allocator, bufferInfo, allocCreateInfo};
//     auto allocInfo = vertexBuffer.getAllocationInfo();
//     auto alloc = vertexBuffer.getAllocation();

//     if (allocInfo.pMappedData) {
//         std::memcpy(allocInfo.pMappedData, indices.data(), sizeof(vertices[0]) * vertices.size());
//     } else {
//         void* data = vertexBuffer.mapMemory();
//         std::memcpy(data, indices.data(), sizeof(indices[0]) * indices.size());
//         vertexBuffer.unmapMemory();
//     }
// }

// }  // namespace Solaris::Graphics::Vulkan