#pragma once

#include <vk_mem_alloc.hpp>
#include <vk_mem_alloc_handles.hpp>

namespace Solaris::Graphics::Vulkan {

class Allocator {
   public:
    Allocator(){};
    Allocator(vma::Allocator inner) : mAllocator(inner) {}
    ~Allocator() { mAllocator.destroy(); }

    Allocator(Allocator&& rhs) noexcept { *this = std::move(rhs); }
    Allocator& operator=(Allocator&& other) noexcept {
        if (this != &other) {
            if (mAllocator) {
                mAllocator.destroy();
            }
            mAllocator = other.mAllocator;
            other.mAllocator = nullptr;
        }
        return *this;
    }

    Allocator(const Allocator&) = delete;
    Allocator& operator=(const Allocator&) = delete;

    vma::Allocator& operator*() { return mAllocator; }
    const vma::Allocator& operator*() const { return mAllocator; }

   private:
    vma::Allocator mAllocator{nullptr};
};

}  // namespace Solaris::Graphics::Vulkan