#include "Graphics/Vulkan/Swapchain.hpp"
#include "Graphics/Vulkan/Context.hpp"
#include "Graphics/Vulkan/QueueFamily.hpp"

#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_raii.hpp>

namespace Solaris::Graphics::Vulkan {

auto chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats) -> vk::SurfaceFormatKHR {
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == vk::Format::eB8G8R8A8Srgb &&
            availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
            return availableFormat;
        }
    }
    return availableFormats[0];
}

auto chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes) -> vk::PresentModeKHR {
    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == vk::PresentModeKHR::eMailbox) {
            return availablePresentMode;
        }
    }

    return vk::PresentModeKHR::eFifo;
}

auto chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities, GLFWwindow* window) -> vk::Extent2D {
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    } else {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        VkExtent2D actualExtent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};

        actualExtent.width =
            std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height =
            std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
}

SwapchainSupportDetails QuerySwapChainSupport(const vk::raii::SurfaceKHR& surface,
                                              const vk::raii::PhysicalDevice& device) {
    SwapchainSupportDetails details;
    details.formats = device.getSurfaceFormatsKHR(surface);
    details.capabilities = device.getSurfaceCapabilitiesKHR(surface);
    details.presentModes = device.getSurfacePresentModesKHR(surface);
    return details;
}

void Context::initSwapchain(GLFWwindow* window) {
    auto swapChainSupport = QuerySwapChainSupport(surface, physicalDevice);

    auto format = chooseSwapSurfaceFormat(swapChainSupport.formats);
    auto presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    auto extent = chooseSwapExtent(swapChainSupport.capabilities, window);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    vk::SwapchainCreateInfoKHR si({}, surface, imageCount, format.format, format.colorSpace, extent, 1,
                                  vk::ImageUsageFlagBits::eColorAttachment);
    auto indices = FindQueueFamilies(physicalDevice, surface);
    uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    if (indices.graphicsFamily != indices.presentFamily) {
        si.imageSharingMode = vk::SharingMode::eConcurrent;
        si.queueFamilyIndexCount = 2;
        si.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        si.imageSharingMode = vk::SharingMode::eExclusive;
        si.queueFamilyIndexCount = 0;
        si.pQueueFamilyIndices = nullptr;
    }

    si.setPreTransform(swapChainSupport.capabilities.currentTransform);
    si.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque);
    si.setPresentMode(presentMode);
    si.setClipped(vk::True);
    si.setOldSwapchain(VK_NULL_HANDLE);

    swapchain = {device, si};
    swapchainImages = swapchain.getImages();

    swapchainFormat = format.format;
    swapchainExtent = extent;
}

void Context::initSwapchainResources() {
    // ImageViews
    swapchainViews.clear();
    swapchainViews.reserve(swapchainImages.size());

    for (const auto image : swapchainImages) {
        vk::ImageViewCreateInfo ci{};
        ci.setImage(image);
        ci.setViewType(vk::ImageViewType::e2D);
        ci.setFormat(swapchainFormat);
        ci.setComponents({vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity,
                          vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity});
        ci.setSubresourceRange({vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1});
        swapchainViews.emplace_back(device, ci);
    }

    // Framebuffers
    swapchainFramebuffers.clear();
    swapchainFramebuffers.reserve(swapchainViews.size());

    for (size_t i = 0; i < swapchainViews.size(); i++) {
        vk::ImageView attachments[] = {swapchainViews[i]};

        vk::FramebufferCreateInfo frambufferInfo{};
        frambufferInfo.setRenderPass(renderPass);
        frambufferInfo.setAttachmentCount(1);
        frambufferInfo.setPAttachments(attachments);
        frambufferInfo.setWidth(swapchainExtent.width);
        frambufferInfo.setHeight(swapchainExtent.height);
        frambufferInfo.setLayers(1);

        swapchainFramebuffers.emplace_back(device, frambufferInfo);
    }
}

}  // namespace Solaris::Graphics::Vulkan