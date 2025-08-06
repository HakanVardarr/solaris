#include "core/InstanceManager.hpp"

#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_funcs.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>

#include <cstdint>
#include <expected>
#include <vector>
#include <vulkan/vulkan_to_string.hpp>

auto checkValidationLayerSupport() -> std::expected<bool, InstanceManagerError> {
    uint32_t layerCount;
    if (auto result = vk::enumerateInstanceLayerProperties(&layerCount, nullptr); result != vk::Result::eSuccess) {
        return std::unexpected(
            InstanceManagerError{.mErrorCode = InstanceManagerError::ErrorCode::eInstanceLayerProperties,
                                 .mErrorMessage = vk::to_string(result)});
    }

    std::vector<vk::LayerProperties> availableLayers(layerCount);
    if (auto result = vk::enumerateInstanceLayerProperties(&layerCount, availableLayers.data());
        result != vk::Result::eSuccess) {
        return std::unexpected(
            InstanceManagerError{.mErrorCode = InstanceManagerError::ErrorCode::eInstanceLayerProperties,
                                 .mErrorMessage = vk::to_string(result)});
    }

    for (const char* layerName : validationLayers) {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) {
            return false;
        }
    }

    return true;
}

auto getRequiredExtensions() -> std::vector<const char*> {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
    if (enableValidationLayers) {
        extensions.push_back(vk::EXTDebugUtilsExtensionName);
    }

    extensions.emplace_back(vk::KHRPortabilityEnumerationExtensionName);
    return extensions;
}

auto InstanceManager::CreateInstance() -> EXPECT_VOID(InstanceManagerError) {
    spdlog::debug("Creating Vulkan instance...");

    vk::ApplicationInfo appInfo{};
    appInfo.sType = vk::StructureType::eApplicationInfo;
    appInfo.pApplicationName = "Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Solaris";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    spdlog::debug("App Name: {}, Version: {}.{}.{}", appInfo.pApplicationName,
                  VK_VERSION_MAJOR(appInfo.applicationVersion), VK_VERSION_MINOR(appInfo.applicationVersion),
                  VK_VERSION_PATCH(appInfo.applicationVersion));
    spdlog::debug("Engine: {}, Version: {}.{}.{}", appInfo.pEngineName, VK_VERSION_MAJOR(appInfo.engineVersion),
                  VK_VERSION_MINOR(appInfo.engineVersion), VK_VERSION_PATCH(appInfo.engineVersion));
    spdlog::debug("Target Vulkan API Version: {}.{}.{}", VK_VERSION_MAJOR(appInfo.apiVersion),
                  VK_VERSION_MINOR(appInfo.apiVersion), VK_VERSION_PATCH(appInfo.apiVersion));

    vk::InstanceCreateInfo createInfo{};
    createInfo.sType = vk::StructureType::eInstanceCreateInfo;
    createInfo.pApplicationInfo = &appInfo;

    if (auto result = checkValidationLayerSupport(); enableValidationLayers && !result) {
        return std::unexpected(
            InstanceManagerError{.mErrorCode = InstanceManagerError::ErrorCode::eValidationLayersNotAvailable,
                                 .mErrorMessage = fmt::format("{}", result.error())});
    }

    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    } else {
        createInfo.enabledLayerCount = 0;
    }

    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    uint32_t extensionCount = 0;
    if (auto result = vk::enumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        result != vk::Result::eSuccess) {
        return std::unexpected(InstanceManagerError{.mErrorCode = InstanceManagerError::ErrorCode::eEnumerateExtensions,
                                                    .mErrorMessage = vk::to_string(result)});
    }

    std::vector<vk::ExtensionProperties> extensions(extensionCount);
    if (auto result = vk::enumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
        result != vk::Result::eSuccess) {
        return std::unexpected(InstanceManagerError{.mErrorCode = InstanceManagerError::ErrorCode::eEnumerateExtensions,
                                                    .mErrorMessage = vk::to_string(result)});
    }

    spdlog::debug("Available Extensions:");
    for (const auto& extension : extensions) {
        spdlog::debug("\t{}", extension.extensionName.data());
    }

    auto requiredExtensions = getRequiredExtensions();
    spdlog::debug("Required Extensions:");
    for (const char* required : requiredExtensions) {
        spdlog::debug("\t{}", required);
        bool found = std::ranges::any_of(
            extensions, [&](const auto& ext) { return std::string_view(ext.extensionName.data()) == required; });

        if (!found) {
            return std::unexpected(
                InstanceManagerError{.mErrorCode = InstanceManagerError::ErrorCode::eRequiredExtensionDoesNotExists,
                                     .mErrorMessage = std::format("Missing required Vulkan extension: {}", required)});
        }
    }

    createInfo.flags |= vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
    createInfo.ppEnabledExtensionNames = requiredExtensions.data();

    if (auto result = vk::createInstance(&createInfo, nullptr, &mInstance); result != vk::Result::eSuccess) {
        return std::unexpected(
            InstanceManagerError{.mErrorCode = InstanceManagerError::ErrorCode::eFailedToCreateInstance,
                                 .mErrorMessage = vk::to_string(result)});
    }

    return {};
}