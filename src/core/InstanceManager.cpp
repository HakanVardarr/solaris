#include "core/InstanceManager.hpp"

#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_funcs.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>

#include <cstdint>
#include <expected>
#include <vector>

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

    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> requiredExtensions;
    for (size_t i = 0; i < glfwExtensionCount; ++i) {
        requiredExtensions.emplace_back(glfwExtensions[i]);
    }
    requiredExtensions.emplace_back(vk::KHRPortabilityEnumerationExtensionName);

    uint32_t extensionCount = 0;
    if (auto result = vk::enumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        result != vk::Result::eSuccess) {
        return std::unexpected(InstanceManagerError{.errorCode = InstanceManagerError::ErrorCode::EnumerateExtensions,
                                                    .errorMessage = vk::to_string(result)});
    }

    std::vector<vk::ExtensionProperties> extensions(extensionCount);
    if (auto result = vk::enumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
        result != vk::Result::eSuccess) {
        return std::unexpected(InstanceManagerError{
            .errorCode = InstanceManagerError::ErrorCode::EnumerateExtensions,
            .errorMessage = vk::to_string(result),
        });
    }

    spdlog::debug("Available Extensions:");
    for (const auto& extension : extensions) {
        spdlog::debug("\t{}", extension.extensionName.data());
    }

    for (const char* required : requiredExtensions) {
        bool found = std::ranges::any_of(
            extensions, [&](const auto& ext) { return std::string_view(ext.extensionName.data()) == required; });

        if (!found) {
            return std::unexpected(InstanceManagerError{
                .errorCode = InstanceManagerError::ErrorCode::RequiredExtensionDoesNotExists,
                .errorMessage = std::format("Missing required Vulkan extension: {}", required),
            });
        }
    }

    createInfo.flags |= vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
    createInfo.ppEnabledExtensionNames = requiredExtensions.data();

    if (auto result = vk::createInstance(&createInfo, nullptr, &mInstance); result != vk::Result::eSuccess) {
        return std::unexpected(InstanceManagerError{
            .errorCode = InstanceManagerError::ErrorCode::FailedToCreateInstance,
            .errorMessage = vk::to_string(result),
        });
    }

    return {};
}