#include "Graphics/Vulkan/Allocator.hpp"
#include "Graphics/Vulkan/Context.hpp"
#include "Graphics/Vulkan/Vertex.hpp"
#include "Graphics/Vulkan/VmaBuffer.hpp"

#include <vk_mem_alloc_enums.hpp>
#include <vk_mem_alloc_handles.hpp>
#include <vk_mem_alloc_structs.hpp>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_raii.hpp>
#include <vulkan/vulkan_structs.hpp>

#include <cstring>

namespace Solaris::Graphics::Vulkan {

void Context::initVertexBuffer() {
    size_t bufferSize = sizeof(vertices[0]) * vertices.size();

    VmaBuffer stagingBuffer;
    stagingBuffer.init(&*allocator, bufferSize, vk::BufferUsageFlagBits::eTransferSrc, true);
    auto allocInfo = vertexBuffer.getAllocationInfo();
    auto alloc = vertexBuffer.getAllocation();

    if (allocInfo.pMappedData) {
        std::memcpy(allocInfo.pMappedData, vertices.data(), sizeof(vertices[0]) * vertices.size());
    } else {
        void* data = stagingBuffer.mapMemory();
        std::memcpy(data, vertices.data(), sizeof(vertices[0]) * vertices.size());
        stagingBuffer.unmapMemory();
    }

    vertexBuffer.init(&*allocator, bufferSize,
                      vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer, false, true);
    copyBuffer(stagingBuffer.getBuffer(), vertexBuffer.getBuffer(), bufferSize);

    stagingBuffer.destroy();
}

void Context::copyBuffer(vk::Buffer src, vk::Buffer dst, vk::DeviceSize size) {
    vk::CommandBufferAllocateInfo allocInfo{};
    allocInfo.setLevel(vk::CommandBufferLevel::ePrimary);
    allocInfo.setCommandPool(*commandPool);
    allocInfo.setCommandBufferCount(1);

    auto commandBuffers = device.allocateCommandBuffers(allocInfo);
    vk::CommandBuffer cmd = commandBuffers[0];

    vk::CommandBufferBeginInfo beginInfo{vk::CommandBufferUsageFlagBits::eOneTimeSubmit};
    cmd.begin(beginInfo);

    vk::BufferCopy copyRegion{0, 0, size};
    cmd.copyBuffer(src, dst, copyRegion);

    cmd.end();

    vk::SubmitInfo submitInfo{};
    submitInfo.setCommandBuffers(cmd);

    graphicsQueue.submit(submitInfo);
    graphicsQueue.waitIdle();
}

}  // namespace Solaris::Graphics::Vulkan