#pragma once
#include <vk_mem_alloc.hpp>
#include <vk_mem_alloc_enums.hpp>
#include <vk_mem_alloc_structs.hpp>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_structs.hpp>

class VmaBuffer {
   public:
    VmaBuffer() = default;

    ~VmaBuffer() { destroy(); }

    VmaBuffer(VmaBuffer&& other) noexcept { *this = std::move(other); }
    VmaBuffer& operator=(VmaBuffer&& other) noexcept {
        if (this != &other) {
            destroy();
            mAllocator = other.mAllocator;
            mBuffer = other.mBuffer;
            mAllocation = other.mAllocation;
            mAllocInfo = other.mAllocInfo;

            other.mBuffer = VK_NULL_HANDLE;
            other.mAllocation = nullptr;
        }
        return *this;
    }

    VmaBuffer(const VmaBuffer&) = delete;
    VmaBuffer& operator=(const VmaBuffer&) = delete;

    [[nodiscard]] vk::Buffer getBuffer() const { return mBuffer; }
    [[nodiscard]] vma::Allocation getAllocation() const { return mAllocation; }
    [[nodiscard]] const vma::AllocationInfo& getAllocationInfo() const { return mAllocInfo; }

    void init(vma::Allocator* allocator,
              vk::DeviceSize size,
              vk::BufferUsageFlags usage,
              bool hostVisible = false,
              bool deviceLocal = false) {
        mAllocator = allocator;

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

        auto [buffer, alloc] = mAllocator->createBuffer(bufferInfo, allocCreateInfo, mAllocInfo);
        mBuffer = buffer;
        mAllocation = alloc;
    }

    void* mapMemory() { return mAllocator->mapMemory(mAllocation); }
    void unmapMemory() { mAllocator->unmapMemory(mAllocation); }

    void destroy() {
        if (mBuffer) {
            mAllocator->destroyBuffer(mBuffer, mAllocation);
            mBuffer = VK_NULL_HANDLE;
            mAllocation = nullptr;
        }
    }

   private:
    vma::Allocator* mAllocator = nullptr;
    vk::Buffer mBuffer{VK_NULL_HANDLE};
    vma::Allocation mAllocation{};
    vma::AllocationInfo mAllocInfo{};
};