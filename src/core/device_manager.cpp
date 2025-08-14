#include "core/device_manager.hpp"
#include "core/queue_family.hpp"
#include "core/surface_manager.hpp"
#include "core/vulkan_context.hpp"
#include "errors/device_manager_error.hpp"
#include "macros.hpp"

#include <spdlog/spdlog.h>
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_raii.hpp>
#include <vulkan/vulkan_structs.hpp>
#include <vulkan/vulkan_to_string.hpp>

#include <map>
#include <set>

namespace solaris::core {

using solaris::errors::DeviceManagerError;

auto DeviceManager::pickPhysicalDevice(VulkanContext& ctx) -> EXPECT_VOID(DeviceManagerError) {
    auto result = ctx.mInstance.enumeratePhysicalDevices();
    if (!result.has_value()) {
        return std::unexpected(
            DeviceManagerError(DeviceManagerError::ErrorCode::eFailedToEnumerate, vk::to_string(result.error())));
    }

    std::vector<vk::raii::PhysicalDevice> devices = std::move(result.value());
    if (devices.empty()) {
        return std::unexpected(DeviceManagerError(DeviceManagerError::ErrorCode::eFailToFindGPU));
    }

    std::multimap<int, vk::raii::PhysicalDevice> candidates;

    for (auto& device : devices) {
        int score = rateDeviceSuitability(device, ctx);
        candidates.emplace(score, std::move(device));
    }

    auto best = candidates.rbegin();
    if (best->first <= 0) {
        return std::unexpected(DeviceManagerError(DeviceManagerError::ErrorCode::eFailToFindGPU));
    }

    auto& bestDevice = best->second;
    auto deviceProperties = bestDevice.getProperties();
    spdlog::debug("Selected GPU: {}", deviceProperties.deviceName.data());

    ctx.mPhysicalDevice = std::move(bestDevice);
    return {};
}

auto DeviceManager::createLogicalDevice(VulkanContext& ctx) -> EXPECT_VOID(DeviceManagerError) {
    auto indices = findQueueFamilies(ctx.mPhysicalDevice, ctx);
    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        vk::DeviceQueueCreateInfo queueCreateInfo({}, queueFamily, 1, &queuePriority);
        queueCreateInfos.push_back(queueCreateInfo);
    }

    vk::PhysicalDeviceFeatures deviceFeatures{};
    vk::DeviceCreateInfo createInfo({}, static_cast<uint32_t>(queueCreateInfos.size()), queueCreateInfos.data(), {}, {},
                                    static_cast<uint32_t>(deviceExtensions.size()), deviceExtensions.data(),
                                    &deviceFeatures);

    if (ctx.validationEnabled) {
        createInfo.setEnabledLayerCount(static_cast<uint32_t>(validationLayers.size()));
        createInfo.setPpEnabledLayerNames(validationLayers.data());
    } else {
        createInfo.enabledLayerCount = 0;
    }

    auto raw = ctx.mPhysicalDevice.createDevice(createInfo);
    if (!raw.has_value()) {
        std::unexpected(
            DeviceManagerError(DeviceManagerError::ErrorCode::eFailedToCreateDevice, vk::to_string(raw.error())));
    }

    ctx.mDevice.swap(raw.value());

    if (auto err = ctx.mDevice.getQueue(indices.graphicsFamily.value(), 0); err.has_value()) {
        ctx.mGraphicsQueue.swap(err.value());
    } else {
        std::unexpected(
            DeviceManagerError(DeviceManagerError::ErrorCode::eFailedToGetQueue, vk::to_string(err.error())));
    }

    if (auto err = ctx.mDevice.getQueue(indices.presentFamily.value(), 0); err.has_value()) {
        ctx.mPresentQueue.swap(err.value());
    } else {
        std::unexpected(
            DeviceManagerError(DeviceManagerError::ErrorCode::eFailedToGetQueue, vk::to_string(err.error())));
    }

    return {};
}

auto DeviceManager::rateDeviceSuitability(const vk::raii::PhysicalDevice& device, VulkanContext& ctx) -> int {
    int score = 0;

    auto deviceProperties = device.getProperties();
    auto deviceFeatures = device.getFeatures();
    auto indices = findQueueFamilies(device, ctx);

    bool extensionsSupported = checkDeviceExtensionSupport(device);
    bool swapChainAdequate = false;

    if (extensionsSupported) {
        auto swapChainSupport = SurfaceManager::QuerySwapChainSupport(ctx, device);
        swapChainAdequate = !swapChainSupport->formats.empty() && !swapChainSupport->presentModes.empty();
    }

    if (!indices.IsComplete() || !extensionsSupported || !swapChainAdequate) {
        return 0;
    }

    if (deviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
        score += 1000;
    }

    score += deviceProperties.limits.maxImageDimension2D;

    return score;
}

auto DeviceManager::checkDeviceExtensionSupport(const vk::raii::PhysicalDevice& device) -> bool {
    auto availableExtensions = device.enumerateDeviceExtensionProperties();

    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());
    for (const auto& extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

}  // namespace solaris::core
