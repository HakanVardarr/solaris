#include "Graphics/Vulkan/Context.hpp"
#include "Graphics/Vulkan/DeviceManager.hpp"
#include "Graphics/Vulkan/SurfaceManager.hpp"

#include <GLFW/glfw3.h>
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
#include <vector>

VKAPI_ATTR VkBool32 VKAPI_CALL debugUtilsMessengerCallback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
                                                           VkDebugUtilsMessageTypeFlagsEXT message_type,
                                                           const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
                                                           void* user_data) {
    spdlog::warn("[Vulkan] {} - {}: {}", callback_data->messageIdNumber, callback_data->pMessageIdName,
                 callback_data->pMessage);

    if (message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        spdlog::warn("[Vulkan] {} - {}: {}", callback_data->messageIdNumber, callback_data->pMessageIdName,
                     callback_data->pMessage);

    } else if (message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        spdlog::error("[Vulkan] {} - {}: {}", callback_data->messageIdNumber, callback_data->pMessageIdName,
                      callback_data->pMessage);
    }
    return VK_FALSE;
}

namespace Solaris::Graphics::Vulkan {

auto VulkanContext::init(GLFWwindow* pWindow) -> void {
    createInstance();
    SurfaceManager::Create(*this, pWindow);
    DeviceManager::pickPhysicalDevice(*this);

    // auto deviceProperties = mPhysicalDevice.getProperties();
}

auto VulkanContext::createInstance() -> void {
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

    vk::InstanceCreateInfo instanceCreateInfo({}, &applicationInfo, {}, {}, static_cast<uint32_t>(extensions.size()),
                                              extensions.data(), nullptr);

#if defined(__APPLE__) || defined(__MACH__)
    instanceCreateInfo.setFlags(vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR);
#endif

    if (validationEnabled) {
        instanceCreateInfo.setEnabledLayerCount(static_cast<uint32_t>(validationLayers.size()));
        instanceCreateInfo.setPpEnabledLayerNames(validationLayers.data());
    }

    mInstance = vk::raii::Instance(mCtx, instanceCreateInfo, nullptr);

    if (validationEnabled) {
        vk::DebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo(
            {},
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eError | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose,
            vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
                vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral,
            reinterpret_cast<vk::PFN_DebugUtilsMessengerCallbackEXT>(debugUtilsMessengerCallback), nullptr);

        auto debugMessenger = mInstance.createDebugUtilsMessengerEXT(debugMessengerCreateInfo);
        mDebugMessenger.swap(debugMessenger);
    }
}

auto VulkanContext::checkValidationLayerSupport() -> bool {
    std::vector<vk::LayerProperties> availableLayers = mCtx.enumerateInstanceLayerProperties();

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

}  // namespace Solaris::Graphics::Vulkan