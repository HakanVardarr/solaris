#include "core/vulkan_context.hpp"
#include "core/device_manager.hpp"
#include "core/surface_manager.hpp"
#include "errors/vulkan_context_error.hpp"

#include "fmt/formatters.hpp"  // IWYU pragma: keep
#include "macros.hpp"

#include <GLFW/glfw3.h>
#include <__expected/unexpected.h>
#include <spdlog/spdlog.h>
#include <vulkan/vk_enum_string_helper.h>
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_raii.hpp>
#include <vulkan/vulkan_structs.hpp>
#include <vulkan/vulkan_to_string.hpp>

#include <algorithm>
#include <expected>
#include <vector>

VKAPI_ATTR VkBool32 VKAPI_CALL debugUtilsMessengerCallback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
                                                           VkDebugUtilsMessageTypeFlagsEXT message_type,
                                                           const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
                                                           void* user_data) {
    if (message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        spdlog::warn("[Vulkan] {} - {}: {}", callback_data->messageIdNumber, callback_data->pMessageIdName,
                     callback_data->pMessage);

    } else if (message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        spdlog::error("[Vulkan] {} - {}: {}", callback_data->messageIdNumber, callback_data->pMessageIdName,
                      callback_data->pMessage);
    }
    return VK_FALSE;
}

namespace solaris::core {

using solaris::errors::VulkanContextError;

auto VulkanContext::init(GLFWwindow* pWindow) -> EXPECT_VOID(VulkanContextError) {
    if (auto result = createInstance(); !result) {
        return std::unexpected(result.error());
    }

    if (auto result = SurfaceManager::Create(*this, pWindow); !result) {
        return std::unexpected(
            VulkanContextError(VulkanContextError::ErrorCode::eSurfaceManager, fmt::format("{}", result.error())));
    }

    if (auto result = DeviceManager::pickPhysicalDevice(*this); !result) {
        return std::unexpected(
            VulkanContextError(VulkanContextError::ErrorCode::eDeviceManager, fmt::format("{}", result.error())));
    }

    auto deviceProperties = mPhysicalDevice.getProperties();

    return {};
}

auto VulkanContext::createInstance() -> EXPECT_VOID(VulkanContextError) {
    auto extensions = getRequiredExtensions();

    vk::ApplicationInfo applicationInfo{"Triangle", 1, "Solaris", 1, VK_API_VERSION_1_1};

    spdlog::debug("Application Name: {}", applicationInfo.pApplicationName);
    spdlog::debug("Application Version: {}.{}.{}", VK_VERSION_MAJOR(applicationInfo.applicationVersion),
                  VK_VERSION_MINOR(applicationInfo.applicationVersion),
                  VK_VERSION_PATCH(applicationInfo.applicationVersion));
    spdlog::debug("Engine Name: {}", applicationInfo.pEngineName);
    spdlog::debug("Engine Version: {}.{}.{}", VK_VERSION_MAJOR(applicationInfo.engineVersion),
                  VK_VERSION_MINOR(applicationInfo.engineVersion), VK_VERSION_PATCH(applicationInfo.engineVersion));
    spdlog::debug("API Version: {}.{}.{}", VK_VERSION_MAJOR(applicationInfo.apiVersion),
                  VK_VERSION_MINOR(applicationInfo.apiVersion), VK_VERSION_PATCH(applicationInfo.apiVersion));

    vk::InstanceCreateInfo instanceCreateInfo(vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR, &applicationInfo,
                                              {}, {}, static_cast<uint32_t>(extensions.size()), extensions.data(),
                                              nullptr);

    if (validationEnabled) {
        instanceCreateInfo.setEnabledLayerCount(static_cast<uint32_t>(validationLayers.size()));
        instanceCreateInfo.setPpEnabledLayerNames(validationLayers.data());
    }

    auto instance = vk::createInstance(instanceCreateInfo, nullptr);
    if (instance.result != vk::Result::eSuccess) {
        return std::unexpected(VulkanContextError(VulkanContextError::ErrorCode::eEnumerateInstanceProperties,
                                                  vk::to_string(instance.result)));
    }

    mInstance = vk::raii::Instance(mCtx, instance.value, nullptr);
    if (validationEnabled) {
        vk::DebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo(
            {},
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eError | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose,
            vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
                vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral,
            debugUtilsMessengerCallback, nullptr);

        auto result = mInstance.createDebugUtilsMessengerEXT(debugMessengerCreateInfo);

        if (!result) {
            spdlog::info("{}", vk::to_string(result.error()));
            return std::unexpected(
                VulkanContextError(VulkanContextError::ErrorCode::eDebugMessenger, vk::to_string(result.error())));
        }
        mDebugMessenger.swap(result.value());
    }

    return {};
}

auto VulkanContext::checkValidationLayerSupport() -> std::expected<bool, VulkanContextError> {
    uint32_t layerCount;
    if (auto result = vk::enumerateInstanceLayerProperties(&layerCount, nullptr); result != vk::Result::eSuccess) {
        return std::unexpected(
            VulkanContextError(VulkanContextError::ErrorCode::eEnumerateInstanceProperties, vk::to_string(result)));
    }

    std::vector<vk::LayerProperties> availableLayers(layerCount);
    if (auto result = vk::enumerateInstanceLayerProperties(&layerCount, availableLayers.data());
        result != vk::Result::eSuccess) {
        return std::unexpected(
            VulkanContextError(VulkanContextError::ErrorCode::eEnumerateInstanceProperties, vk::to_string(result)));
    }

    for (auto layerName : validationLayers) {
        bool found = std::ranges::any_of(availableLayers,
                                         [&](const auto& layer) { return std::strcmp(layerName, layer.layerName); });
        if (!found) {
            return false;
        }
    }

    return true;
}

auto VulkanContext::getRequiredExtensions() -> std::vector<const char*> {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
    if (validationEnabled) {
        extensions.push_back(vk::EXTDebugUtilsExtensionName);
    }
#if defined(__APPLE__) || defined(__MACH__)
    extensions.emplace_back(vk::KHRPortabilityEnumerationExtensionName);
#endif

    return extensions;
}

}  // namespace solaris::core