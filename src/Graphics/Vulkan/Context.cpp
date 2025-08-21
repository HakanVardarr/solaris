#include "Graphics/Vulkan/Context.hpp"
#include "Graphics/Vulkan/QueueFamily.hpp"
#include "Graphics/Vulkan/Swapchain.hpp"

#include <GLFW/glfw3.h>

#include <spdlog/spdlog.h>

#include <vulkan/vk_enum_string_helper.h>
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_raii.hpp>
#include <vulkan/vulkan_structs.hpp>

#include <vk_mem_alloc.hpp>
#include <vk_mem_alloc_handles.hpp>
#include <vk_mem_alloc_structs.hpp>

#include <map>
#include <set>
#include <stdexcept>
#include <vector>

VKAPI_ATTR VkBool32 VKAPI_CALL debugUtilsMessengerCallback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
                                                           VkDebugUtilsMessageTypeFlagsEXT message_type,
                                                           const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
                                                           void* /*user_data*/) {
    spdlog::level::level_enum level = spdlog::level::info;
    if (message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) {
        level = spdlog::level::debug;
    } else if (message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
        level = spdlog::level::info;
    } else if (message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        level = spdlog::level::warn;
    } else if (message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        level = spdlog::level::err;
    }

    std::string typeStr;
    if (message_type & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT)
        typeStr += "General ";
    if (message_type & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT)
        typeStr += "Validation ";
    if (message_type & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT)
        typeStr += "Performance ";

    spdlog::log(level, "[Vulkan][{}] {} - {}: {}", typeStr, callback_data->messageIdNumber,
                callback_data->pMessageIdName ? callback_data->pMessageIdName : "NoName", callback_data->pMessage);

    for (uint32_t i = 0; i < callback_data->objectCount; i++) {
        const auto& obj = callback_data->pObjects[i];
        spdlog::debug("    Object[{}]: type={} handle={:#x} name={}", i, string_VkObjectType(obj.objectType),
                      (uint64_t)obj.objectHandle, obj.pObjectName ? obj.pObjectName : "Unnamed");
    }

    if (message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        spdlog::critical("Fatal Vulkan error, terminating...");
        std::exit(1);
    }

    return VK_FALSE;
}

namespace Solaris::Graphics::Vulkan {

const std::vector<const char*> validationLayers = {"VK_LAYER_KHRONOS_validation"};
#if defined(__APPLE__) || defined(__MACH__)
const std::vector<const char*> deviceExtensions = {vk::KHRSwapchainExtensionName, "VK_KHR_portability_subset"};
#else
const std::vector<const char*> deviceExtensions = {vk::KHRSwapchainExtensionName};
#endif

[[nodiscard]] std::vector<const char*> getRequiredExtensions(const bool& validationEnabled) {
    uint32_t glfwExtensionsCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionsCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionsCount);
    if (validationEnabled) {
        extensions.push_back(vk::EXTDebugUtilsExtensionName);
    }
#if defined(__APPLE__) || defined(__MACH__)
    extensions.emplace_back(vk::KHRPortabilityEnumerationExtensionName);
#endif
    return extensions;
}

[[nodiscard]] bool checkDeviceExtensionSupport(const vk::raii::PhysicalDevice& device) {
    auto availableExtensions = device.enumerateDeviceExtensionProperties();
    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

    for (const auto& extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

[[nodiscard]] int rateDeviceSuitability(const vk::raii::SurfaceKHR& surface, const vk::raii::PhysicalDevice& device) {
    int score = 0;

    auto deviceProperties = device.getProperties();
    auto deviceFeatures = device.getFeatures();
    auto indices = FindQueueFamilies(device, surface);

    bool extensionsSupported = checkDeviceExtensionSupport(device);
    bool swapChainAdequate = false;

    if (extensionsSupported) {
        auto swapChainSupport = QuerySwapChainSupport(surface, device);
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }

    if (!indices.isComplete() || !extensionsSupported || !swapChainAdequate) {
        return 0;
    }

    if (deviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
        score += 1000;
    }

    score += deviceProperties.limits.maxImageDimension2D;
    return score;

    return score;
}

void Context::init(GLFWwindow* window) {
    // Instance + Devices
    initCore(window);
    // Swapchain
    initSwapchain(window);
    // Render Pass + Pipeline
    initPipeline();
    // Views + Framebuffers
    initSwapchainResources();
    // Command Pool + Command Buffer
    initCommands();
    // Sync
    initSync();
}

void Context::initCore(GLFWwindow* window) {
    // Instance
    vk::ApplicationInfo ai{};
    ai.setPApplicationName("Triangle");
    ai.setApplicationVersion(VK_MAKE_VERSION(0, 0, 1));
    ai.setPEngineName("Solaris");
    ai.setEngineVersion(VK_MAKE_VERSION(0, 0, 1));
    ai.setApiVersion(VK_API_VERSION_1_1);

    spdlog::info("Initializing Solaris Vulkan Engine v{}.{}.{}", 0, 0, 1);

    std::vector<const char*> extensions = getRequiredExtensions(validationEnabled);
    vk::InstanceCreateInfo ici{};
    ici.setPApplicationInfo(&ai);
    ici.setEnabledExtensionCount(static_cast<uint32_t>(extensions.size()));
    ici.setPpEnabledExtensionNames(extensions.data());
#if defined(__APPLE__) || defined(__MACH__)
    ici.setFlags(vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR);
#endif
    if (validationEnabled) {
        ici.setEnabledLayerCount(static_cast<uint32_t>(validationLayers.size()));
        ici.setPpEnabledLayerNames(validationLayers.data());
    }
    instance = {vulkanContext, ici, nullptr};

    spdlog::info("Creating Vulkan instance with {} extensions, {} validation layers", extensions.size(),
                 validationLayers.size());
    for (auto& ext : extensions)
        spdlog::debug("  Extension enabled: {}", ext);
    for (auto& layer : validationLayers)
        spdlog::debug("  Validation layer: {}", layer);

    // DebugMessenger
    if (validationEnabled) {
        vk::DebugUtilsMessengerCreateInfoEXT dmi{};
        dmi.setMessageSeverity(vk::DebugUtilsMessageSeverityFlagBitsEXT::eError |
                               vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                               vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose);
        dmi.setMessageType(vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
                           vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
                           vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral);
        dmi.pfnUserCallback = reinterpret_cast<vk::PFN_DebugUtilsMessengerCallbackEXT>(debugUtilsMessengerCallback);
        debugMessenger = instance.createDebugUtilsMessengerEXT(dmi);
    }

    // Surface
    VkSurfaceKHR _surface;
    if (auto result = glfwCreateWindowSurface(*instance, window, nullptr, &_surface); result != VK_SUCCESS) {
        throw std::runtime_error(std::format("Failed to create window surface: {}", string_VkResult(result)));
    }
    surface = {instance, _surface};
    spdlog::info("Window surface created successfully.");

    // Physical Device
    std::vector<vk::raii::PhysicalDevice> devices = instance.enumeratePhysicalDevices();
    if (devices.empty()) {
        throw std::runtime_error("Failed to find GPUs on device.");
    }

    std::multimap<int, vk::raii::PhysicalDevice> candidates;
    for (auto& device : devices) {
        int score = rateDeviceSuitability(surface, device);
        candidates.emplace(score, std::move(device));
    }

    auto best = candidates.rbegin();
    if (best->first <= 0) {
        throw std::runtime_error("Failed to find suitable GPU device.");
    }

    physicalDevice = std::move(best->second);

    auto deviceProperties = physicalDevice.getProperties();
    spdlog::info("Found {} Vulkan-capable devices.", devices.size());
    spdlog::info("Selected device: {} (type: {}, score: {})", deviceProperties.deviceName.data(),
                 string_VkPhysicalDeviceType(static_cast<VkPhysicalDeviceType>(deviceProperties.deviceType)),
                 rateDeviceSuitability(surface, physicalDevice));

    // Logical Device
    auto indices = FindQueueFamilies(physicalDevice, surface);
    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        vk::DeviceQueueCreateInfo di({}, queueFamily, 1, &queuePriority);
        queueCreateInfos.push_back(di);
    }

    spdlog::info("Creating logical device with {} queues.", uniqueQueueFamilies.size());
    spdlog::debug("Graphics queue family: {}", indices.graphicsFamily.value());
    spdlog::debug("Present queue family: {}", indices.presentFamily.value());

    vk::PhysicalDeviceFeatures df{};
    vk::DeviceCreateInfo di{{}, static_cast<uint32_t>(queueCreateInfos.size()), queueCreateInfos.data(), {},
                            {}, static_cast<uint32_t>(deviceExtensions.size()), deviceExtensions.data(), &df};
    if (validationEnabled) {
        di.setEnabledLayerCount(static_cast<uint32_t>(validationLayers.size()));
        di.setPpEnabledLayerNames(validationLayers.data());
    } else {
        di.setEnabledLayerCount(0);
    }

    device = {physicalDevice, di};
    graphicsQueue = device.getQueue(indices.graphicsFamily.value(), 0);
    presentQueue = device.getQueue(indices.presentFamily.value(), 0);

    // Vulkan Memory Allocator
    vma::AllocatorCreateInfo aci{};
    aci.setVulkanApiVersion(physicalDevice.getProperties().apiVersion);
    aci.setPhysicalDevice(*physicalDevice);
    aci.setDevice(*device);
    aci.setInstance(*instance);
    allocator = vma::createAllocator(aci);
}

void Context::recreateSwapchain(GLFWwindow* window) {
    device.waitIdle();
    vk::raii::SwapchainKHR oldSwap = std::move(swapchain);
    destroySwapchainResources();
    initSwapchain(window, oldSwap);
    initSwapchainResources();
}

void Context::destroySwapchainResources() {
    swapchainFramebuffers.clear();
    swapchainViews.clear();
}

}  // namespace Solaris::Graphics::Vulkan