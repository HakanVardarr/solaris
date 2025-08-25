#include "Graphics/Vulkan/Buffer.hpp"

#include <cstdint>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_raii.hpp>
#include <vulkan/vulkan_structs.hpp>

#include <cstring>

namespace Solaris::Graphics::Vulkan {

void Buffer::init(vma::Allocator* _allocator,
                  vk::DeviceSize size,
                  vk::BufferUsageFlags usage,
                  bool hostVisible,
                  bool deviceLocal) {
    allocator = _allocator;

    vk::BufferCreateInfo bufferInfo{};
    bufferInfo.setSize(size);
    bufferInfo.setUsage(usage);
    bufferInfo.setSharingMode(vk::SharingMode::eExclusive);

    vma::AllocationCreateInfo allocCreateInfo{};
    if (deviceLocal) {
        allocCreateInfo.usage = vma::MemoryUsage::eAutoPreferDevice;
    } else {
        allocCreateInfo.usage = vma::MemoryUsage::eAuto;
    }

    if (hostVisible) {
        allocCreateInfo.flags =
            vma::AllocationCreateFlagBits::eHostAccessSequentialWrite | vma::AllocationCreateFlagBits::eMapped;
    }

    auto [buffer, alloc] = allocator->createBuffer(bufferInfo, allocCreateInfo, allocInfo);
    _buffer = buffer;
    allocation = alloc;
}

void Buffer::destroy() {
    if (_buffer) {
        allocator->destroyBuffer(_buffer, allocation);
        _buffer = VK_NULL_HANDLE;
        allocation = nullptr;
    }
}

void Buffer::copy(Buffer& src,
                  Buffer& dst,
                  vk::DeviceSize size,
                  vk::raii::CommandPool& commandPool,
                  vk::raii::Device& device,
                  vk::Queue& graphicsQueue) {
    vk::CommandBufferAllocateInfo allocInfo{};
    allocInfo.setLevel(vk::CommandBufferLevel::ePrimary);
    allocInfo.setCommandPool(*commandPool);
    allocInfo.setCommandBufferCount(1);

    auto commandBuffers = device.allocateCommandBuffers(allocInfo);
    vk::CommandBuffer cmd = commandBuffers[0];

    vk::CommandBufferBeginInfo beginInfo{vk::CommandBufferUsageFlagBits::eOneTimeSubmit};
    cmd.begin(beginInfo);

    vk::BufferCopy copyRegion{0, 0, size};
    cmd.copyBuffer(src.getBuffer(), dst.getBuffer(), copyRegion);

    cmd.end();

    vk::SubmitInfo submitInfo{};
    submitInfo.setCommandBuffers(cmd);

    graphicsQueue.submit(submitInfo);
    graphicsQueue.waitIdle();
}

void IndexBuffer::init(vma::Allocator& _allocator,
                       const std::vector<uint16_t>& indices,
                       vk::raii::CommandPool& commandPool,
                       vk::raii::Device& device,
                       vk::Queue& graphicsQueue) {
    vk::DeviceSize bufferSize = sizeof(indices[0]) * indices.size();
    Buffer staging;
    staging.init(&_allocator, bufferSize, vk::BufferUsageFlagBits::eTransferSrc, true);
    auto allocInfo = staging.getAllocationInfo();
    auto alloc = staging.getAllocation();

    if (allocInfo.pMappedData) {
        std::memcpy(allocInfo.pMappedData, indices.data(), bufferSize);
    } else {
        void* data = staging.mapMemory();
        std::memcpy(data, indices.data(), bufferSize);
        staging.unmapMemory();
    }

    Buffer::init(&_allocator, bufferSize, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer,
                 false, true);

    Buffer::copy(staging, *this, bufferSize, commandPool, device, graphicsQueue);
    indexCount = static_cast<uint32_t>(indices.size());

    staging.destroy();
}

}  // namespace Solaris::Graphics::Vulkan