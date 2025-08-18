// #include "Graphics/Vulkan/Device.hpp"
// #include "Graphics/Vulkan/Context.hpp"
// #include "Graphics/Vulkan/Device.hpp"
// #include "Graphics/Vulkan/QueueFamily.hpp"
// #include "Graphics/Vulkan/SwapChain.hpp"

// #include <spdlog/spdlog.h>
// #include <vulkan/vulkan_core.h>
// #include <vulkan/vulkan_enums.hpp>
// #include <vulkan/vulkan_handles.hpp>
// #include <vulkan/vulkan_raii.hpp>
// #include <vulkan/vulkan_structs.hpp>
// #include <vulkan/vulkan_to_string.hpp>

// #include <map>
// #include <set>
// #include <stdexcept>

// namespace Solaris::Graphics::Vulkan {

// auto checkDeviceExtensionSupport(vk::PhysicalDevice device) -> bool {
//     auto availableExtensions = device.enumerateDeviceExtensionProperties();

//     std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());
//     for (const auto& extension : availableExtensions) {
//         requiredExtensions.erase(extension.extensionName);
//     }

//     return requiredExtensions.empty();
// }

// auto rateDeviceSuitability(VulkanContext& ctx, const vk::raii::PhysicalDevice& device) -> int {
//     int score = 0;

//     auto deviceProperties = device.getProperties();
//     auto deviceFeatures = device.getFeatures();
//     auto indices = FindQueueFamilies(device);

//     bool extensionsSupported = checkDeviceExtensionSupport(device);
//     bool swapChainAdequate = false;

//     if (extensionsSupported) {
//         auto swapChainSupport = QuerySwapChainSupport(ctx, device);
//         swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
//     }

//     if (!indices.isComplete() || !extensionsSupported || !swapChainAdequate) {
//         return 0;
//     }

//     if (deviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
//         score += 1000;
//     }

//     score += deviceProperties.limits.maxImageDimension2D;
//     return score;
// }

// auto PickPhysicalDevice(VulkanContext& ctx) -> void {
//     std::vector<vk::raii::PhysicalDevice> devices = ctx.mInstance.enumeratePhysicalDevices();
//     if (devices.empty()) {
//         throw std::runtime_error("Failed to find GPUs on device.");
//     }

//     std::multimap<int, vk::raii::PhysicalDevice> candidates;
//     for (auto& device : devices) {
//         int score = rateDeviceSuitability(ctx, device);
//         candidates.emplace(score, std::move(device));
//     }

//     auto best = candidates.rbegin();
//     if (best->first <= 0) {
//         throw std::runtime_error("Failed to find suitable GPU device");
//     }

//     auto& bestDevice = best->second;
//     auto deviceProperties = bestDevice.getProperties();
//     spdlog::debug("Selected GPU: {}", deviceProperties.deviceName.data());
//     ctx.mPhysicalDevice = std::move(bestDevice);
// }

// auto CreateLogicalDevice(VulkanContext& ctx) -> void {
//     auto indices = FindQueueFamilies(ctx, ctx.mPhysicalDevice);
//     std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
//     std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

//     float queuePrioity = 1.0f;
//     for (uint32_t queueFamily : uniqueQueueFamilies) {
//         vk::DeviceQueueCreateInfo createInfo({}, queueFamily, 1, &queuePrioity);
//         queueCreateInfos.push_back(createInfo);
//     }

//     vk::PhysicalDeviceFeatures deviceFeatures{};
//     vk::DeviceCreateInfo createInfo({}, static_cast<uint32_t>(queueCreateInfos.size()), queueCreateInfos.data(), {},
//     {},
//                                     static_cast<uint32_t>(deviceExtensions.size()), deviceExtensions.data(),
//                                     &deviceFeatures);

//     if (ctx.validationEnabled) {
//         createInfo.setEnabledLayerCount(static_cast<uint32_t>(validationLayers.size()));
//         createInfo.setPpEnabledLayerNames(validationLayers.data());
//     } else {
//         createInfo.enabledLayerCount = 0;
//     }

//     auto raw = ctx.mPhysicalDevice.createDevice(createInfo);
//     ctx.mDevice.swap(raw);
//     ctx.mGraphicsQueue = ctx.mDevice.getQueue(indices.graphicsFamily.value(), 0);
//     ctx.mPresentQueue = ctx.mDevice.getQueue(indices.presentFamily.value(), 0);
// }

// }  // namespace Solaris::Graphics::Vulkan