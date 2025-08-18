#include "Graphics/Vulkan/SwapChain.hpp"
#include <GLFW/glfw3.h>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_raii.hpp>
#include <vulkan/vulkan_structs.hpp>
#include "Graphics/Vulkan/Context.hpp"
#include "Graphics/Vulkan/QueueFamily.hpp"

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

auto chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities, GLFWwindow* pWindow) -> vk::Extent2D {
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    } else {
        int width, height;
        glfwGetFramebufferSize(pWindow, &width, &height);

        VkExtent2D actualExtent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};

        actualExtent.width =
            std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height =
            std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
}

auto createImageViews(VulkanContext& ctx) -> void {
    ctx.mSwapChainImageViews.clear();
    ctx.mSwapChainImageViews.reserve(ctx.mSwapChainImages.size());

    for (const auto image : ctx.mSwapChainImages) {
        vk::ImageViewCreateInfo createInfo{};
        createInfo.image = image;
        createInfo.viewType = vk::ImageViewType::e2D;
        createInfo.format = ctx.mSwapChainImageFormat;
        createInfo.components = {vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity,
                                 vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity};
        createInfo.subresourceRange = {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1};

        ctx.mSwapChainImageViews.emplace_back(ctx.mDevice, createInfo);
    }
}

auto QuerySwapChainSupport(VulkanContext& ctx, vk::raii::PhysicalDevice device) -> SwapchainSupportDetails {
    SwapchainSupportDetails details;

    details.formats = device.getSurfaceFormatsKHR(ctx.mSurface);
    details.capabilities = device.getSurfaceCapabilitiesKHR(ctx.mSurface);
    details.presentModes = device.getSurfacePresentModesKHR(ctx.mSurface);

    return details;
}

auto CreateSwapChain(VulkanContext& ctx, GLFWwindow* pWindow) -> void {
    auto swapChainSupport = QuerySwapChainSupport(ctx, ctx.mPhysicalDevice);

    auto format = chooseSwapSurfaceFormat(swapChainSupport.formats);
    auto presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    auto extent = chooseSwapExtent(swapChainSupport.capabilities, pWindow);
    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    vk::SwapchainCreateInfoKHR createInfo({}, ctx.mSurface, imageCount, format.format, format.colorSpace, extent, 1,
                                          vk::ImageUsageFlagBits::eColorAttachment);

    QueueFamilyIndices indices = FindQueueFamilies(ctx, ctx.mPhysicalDevice);
    uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    if (indices.graphicsFamily != indices.presentFamily) {
        createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        createInfo.imageSharingMode = vk::SharingMode::eExclusive;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
    createInfo.presentMode = presentMode;
    createInfo.clipped = vk::True;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    ctx.mSwapChain = vk::raii::SwapchainKHR(ctx.mDevice, createInfo);
    ctx.mSwapChainImages = ctx.mSwapChain.getImages();

    ctx.mSwapChainImageFormat = format.format;
    ctx.mSwapChainExtent = extent;

    createImageViews(ctx);
}

auto CreateFrameBuffers(VulkanContext& ctx) -> void {
    ctx.mSwapChainFramebuffers.clear();
    ctx.mSwapChainFramebuffers.reserve(ctx.mSwapChainImageViews.size());

    for (size_t i = 0; i < ctx.mSwapChainImageViews.size(); i++) {
        vk::ImageView attachments[] = {ctx.mSwapChainImageViews[i]};

        vk::FramebufferCreateInfo frambufferInfo{};
        frambufferInfo.setRenderPass(ctx.mRenderPass);
        frambufferInfo.setAttachmentCount(1);
        frambufferInfo.setPAttachments(attachments);
        frambufferInfo.setWidth(ctx.mSwapChainExtent.width);
        frambufferInfo.setHeight(ctx.mSwapChainExtent.height);
        frambufferInfo.setLayers(1);

        ctx.mSwapChainFramebuffers.emplace_back(ctx.mDevice, frambufferInfo);
    }
}

}  // namespace Solaris::Graphics::Vulkan
