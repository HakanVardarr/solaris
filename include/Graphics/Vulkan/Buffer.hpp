#pragma once

#include <vk_mem_alloc.hpp>
#include <vk_mem_alloc_enums.hpp>
#include <vk_mem_alloc_handles.hpp>
#include <vk_mem_alloc_structs.hpp>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_raii.hpp>
#include <vulkan/vulkan_structs.hpp>

namespace Solaris::Graphics::Vulkan {

class Buffer {
   public:
    Buffer() = default;
    virtual ~Buffer() { destroy(); }

    Buffer(Buffer&& other) noexcept { *this = std::move(other); }
    Buffer& operator=(Buffer&& other) noexcept {
        if (this != &other) {
            destroy();
            allocator = other.allocator;
            _buffer = other._buffer;
            allocation = other.allocation;
            allocInfo = other.allocInfo;
            other._buffer = VK_NULL_HANDLE;
            other.allocation = nullptr;
        }
        return *this;
    }

    Buffer(const Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;

    [[nodiscard]] vma::Allocation getAllocation() const { return allocation; }
    [[nodiscard]] const vma::AllocationInfo& getAllocationInfo() const { return allocInfo; }
    [[nodiscard]] vk::Buffer getBuffer() const { return _buffer; }

    void init(vma::Allocator* allocator,
              vk::DeviceSize size,
              vk::BufferUsageFlags usage,
              bool hostVisible = false,
              bool deviceLocal = false);

    void* mapMemory() { return allocator->mapMemory(allocation); }
    void unmapMemory() { allocator->unmapMemory(allocation); }
    void destroy();
    static void copy(Buffer& src,
                     Buffer& dst,
                     vk::DeviceSize size,
                     vk::raii::CommandPool& commandPool,
                     vk::raii::Device& device,
                     vk::Queue& graphicsQueue);

   protected:
    vma::Allocator* allocator = nullptr;
    vk::Buffer _buffer{VK_NULL_HANDLE};
    vma::Allocation allocation{};
    vma::AllocationInfo allocInfo{};
};

class VertexBuffer : Buffer {
   public:
    VertexBuffer() = default;

    template <typename V>
    void init(vma::Allocator& _allocator,
              const std::vector<V>& vertices,
              vk::raii::CommandPool& commandPool,
              vk::raii::Device& device,
              vk::Queue& graphicsQueue) {
        vk::DeviceSize bufferSize = sizeof(V) * vertices.size();

        Buffer staging;
        staging.init(&_allocator, bufferSize, vk::BufferUsageFlagBits::eTransferSrc, true);
        auto allocInfo = staging.getAllocationInfo();
        auto alloc = staging.getAllocation();

        if (allocInfo.pMappedData) {
            std::memcpy(allocInfo.pMappedData, vertices.data(), bufferSize);
        } else {
            void* data = staging.mapMemory();
            std::memcpy(data, vertices.data(), bufferSize);
            staging.unmapMemory();
        }

        Buffer::init(&_allocator, bufferSize,
                     vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer, false, true);

        Buffer::copy(staging, *this, bufferSize, commandPool, device, graphicsQueue);

        vertexCount = static_cast<uint32_t>(vertices.size());
        staging.destroy();
    }

    [[nodiscard]] size_t getVertexCount() const { return vertexCount; }
    [[nodiscard]] vk::Buffer getBuffer() const { return _buffer; }

   private:
    size_t vertexCount;
};

class IndexBuffer : Buffer {
   public:
    IndexBuffer() = default;
    void init(vma::Allocator& _allocator,
              const std::vector<uint16_t>& indices,
              vk::raii::CommandPool& commandPool,
              vk::raii::Device& device,
              vk::Queue& graphicsQueue);

    [[nodiscard]] size_t getIndexCount() const { return indexCount; }
    [[nodiscard]] vk::Buffer getBuffer() const { return _buffer; }

   private:
    size_t indexCount;
};

}  // namespace Solaris::Graphics::Vulkan