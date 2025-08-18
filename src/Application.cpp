#include "Application.hpp"

#include <spdlog/fmt/bundled/format.h>
#include <spdlog/spdlog.h>
#include <vulkan/vk_enum_string_helper.h>
#include <vulkan/vulkan_core.h>
#include <cstdint>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_raii.hpp>

#include <stdexcept>
#include <vulkan/vulkan_to_string.hpp>

auto Application::Run() -> void {
    initWindow();
    initVulkan();
    mainLoop();
}

Application::~Application() {
    if (pWindow != nullptr) {
        glfwDestroyWindow(pWindow);
    }

    glfwTerminate();
}

auto Application::initWindow() -> void {
    const uint32_t WIDTH = 800;
    const uint32_t HEIGHT = 600;

    if (glfwInit() == GLFW_FALSE) {
        throw std::runtime_error("Failed to initialize GLFW");
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    pWindow = glfwCreateWindow(WIDTH, HEIGHT, "solaris", nullptr, nullptr);
    if (pWindow == nullptr) {
        throw std::runtime_error("Failed to create window");
    }
}

auto Application::initVulkan() -> void {
    try {
        mContext.Init(pWindow);
    } catch (vk::SystemError& err) {
        throw std::runtime_error(err.what());
    }
}

auto Application::mainLoop() -> void {
    while (glfwWindowShouldClose(pWindow) == GLFW_FALSE) {
        glfwPollEvents();
        drawFrame();
    }

    mContext.mDevice.waitIdle();
}

auto Application::recordCommandBuffer(uint32_t imageIndex) -> void {
    vk::CommandBufferBeginInfo beginInfo{};
    beginInfo.setPInheritanceInfo(nullptr);

    mContext.mCommandBuffer.begin(beginInfo);

    vk::RenderPassBeginInfo renderPassInfo{};
    renderPassInfo.setRenderPass(mContext.mRenderPass);
    renderPassInfo.setFramebuffer(mContext.mSwapChainFramebuffers[imageIndex]);
    renderPassInfo.renderArea.setOffset({0, 0});
    renderPassInfo.renderArea.setExtent(mContext.mSwapChainExtent);

    vk::ClearValue clearColor;
    clearColor.color = vk::ClearColorValue(std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f});
    renderPassInfo.setClearValueCount(1);
    renderPassInfo.setPClearValues(&clearColor);

    mContext.mCommandBuffer.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
    mContext.mCommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, mContext.mPipeline);

    vk::Viewport viewport{};
    viewport.setX(0.0f);
    viewport.setY(0.0f);
    viewport.setWidth(static_cast<float>(mContext.mSwapChainExtent.width));
    viewport.setHeight(static_cast<float>(mContext.mSwapChainExtent.height));
    viewport.setMinDepth(0.0f);
    viewport.setMaxDepth(1.0f);

    mContext.mCommandBuffer.setViewport(0, {viewport});

    vk::Rect2D scissor{};
    scissor.setOffset({0, 0});
    scissor.setExtent(mContext.mSwapChainExtent);

    mContext.mCommandBuffer.setScissor(0, {scissor});
    mContext.mCommandBuffer.draw(3, 1, 0, 0);

    mContext.mCommandBuffer.endRenderPass();
    mContext.mCommandBuffer.end();
}

auto Application::drawFrame() -> void {
    if (auto result = mContext.mDevice.waitForFences({mContext.mInFlightFence}, vk::True, UINT64_MAX);
        result != vk::Result::eSuccess) {
        throw std::runtime_error(std::format("{}", vk::to_string(result)));
    }
    mContext.mDevice.resetFences({mContext.mInFlightFence});

    // uint32_t imageIndex;
    vk::AcquireNextImageInfoKHR acquireInfo{};
    acquireInfo.setSwapchain(mContext.mSwapChain);
    acquireInfo.setSemaphore(mContext.mImageAvailableSemaphore);
    acquireInfo.setDeviceMask(1);
    acquireInfo.setTimeout(UINT64_MAX);

    auto result = mContext.mDevice.acquireNextImage2KHR(acquireInfo);
    mContext.mCommandBuffer.reset();
    recordCommandBuffer(result.second);

    vk::SubmitInfo submitInfo{};
    vk::Semaphore waitSemaphores[] = {mContext.mImageAvailableSemaphore};
    vk::PipelineStageFlags waitStages[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
    submitInfo.setWaitSemaphoreCount(1);
    submitInfo.setPWaitSemaphores(waitSemaphores);
    submitInfo.setPWaitDstStageMask(waitStages);
    submitInfo.setCommandBufferCount(1);
    submitInfo.setPCommandBuffers(&*(mContext.mCommandBuffer));

    vk::Semaphore signalSemaphores[] = {mContext.mRenderFinishedSemaphore};
    submitInfo.setSignalSemaphoreCount(1);
    submitInfo.setPSignalSemaphores(signalSemaphores);

    mContext.mGraphicsQueue.submit({submitInfo}, mContext.mInFlightFence);

    vk::PresentInfoKHR presentInfo{};
    presentInfo.setWaitSemaphoreCount(1);
    presentInfo.setPWaitSemaphores(signalSemaphores);

    vk::SwapchainKHR swapChains[] = {mContext.mSwapChain};
    presentInfo.setSwapchainCount(1);
    presentInfo.setPSwapchains(swapChains);
    presentInfo.setPImageIndices(&result.second);
    presentInfo.setPResults(nullptr);

    if (auto result = mContext.mPresentQueue.presentKHR(presentInfo); result != vk::Result::eSuccess) {
        throw std::runtime_error(std::format("{}", vk::to_string(result)));
    }
}
