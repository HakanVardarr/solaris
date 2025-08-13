#pragma once
#include "errors/vulkan_context.error.hpp"
#include "macros.hpp"

#include <spdlog/fmt/bundled/format.h>

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

#include <expected>

namespace solaris::core {

using solaris::errors::VulkanContextError;

struct VulkanContext {
    ~VulkanContext() {}

    vk::DispatchLoaderDynamic mDldi{};

    vk::raii::Context mCtx{};
    vk::raii::Instance mInstance{nullptr};
    vk::raii::DebugUtilsMessengerEXT mDebugMessenger{nullptr};

#ifdef NDEBUG
    bool validationEnabled = false;
#else
    bool validationEnabled = true;
#endif

    auto init() -> EXPECT_VOID(VulkanContextError);

   private:
    auto createInstance() -> EXPECT_VOID(VulkanContextError);
    auto checkValidationLayerSupport() -> std::expected<bool, VulkanContextError>;
    auto getRequiredExtensions() -> std::vector<const char*>;
};

}  // namespace solaris::core
