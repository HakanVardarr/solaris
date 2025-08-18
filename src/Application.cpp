#include "Application.hpp"

#include <spdlog/fmt/bundled/format.h>
#include <spdlog/spdlog.h>
#include <vulkan/vk_enum_string_helper.h>
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_raii.hpp>
#include <vulkan/vulkan_to_string.hpp>

#include <cstdint>
#include <stdexcept>

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
        mContext.init(pWindow);
    } catch (vk::SystemError& err) {
        throw std::runtime_error(err.what());
    }
}

auto Application::mainLoop() -> void {
    while (glfwWindowShouldClose(pWindow) == GLFW_FALSE) {
        glfwPollEvents();
        drawFrame();
    }

    mContext.device.waitIdle();
}

auto Application::recordCommandBuffer(uint32_t imageIndex) -> void {
    vk::CommandBufferBeginInfo beginInfo{};
    beginInfo.setPInheritanceInfo(nullptr);

    mContext.commandBuffer.begin(beginInfo);

    vk::RenderPassBeginInfo renderPassInfo{};
    renderPassInfo.setRenderPass(mContext.renderPass);
    renderPassInfo.setFramebuffer(mContext.swapchainFramebuffers[imageIndex]);
    renderPassInfo.renderArea.setOffset({0, 0});
    renderPassInfo.renderArea.setExtent(mContext.swapchainExtent);

    vk::ClearValue clearColor;
    clearColor.color = vk::ClearColorValue(std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f});
    renderPassInfo.setClearValueCount(1);
    renderPassInfo.setPClearValues(&clearColor);

    mContext.commandBuffer.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
    mContext.commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, mContext.pipeline);

    vk::Viewport viewport{};
    viewport.setX(0.0f);
    viewport.setY(0.0f);
    viewport.setWidth(static_cast<float>(mContext.swapchainExtent.width));
    viewport.setHeight(static_cast<float>(mContext.swapchainExtent.height));
    viewport.setMinDepth(0.0f);
    viewport.setMaxDepth(1.0f);

    mContext.commandBuffer.setViewport(0, {viewport});

    vk::Rect2D scissor{};
    scissor.setOffset({0, 0});
    scissor.setExtent(mContext.swapchainExtent);

    mContext.commandBuffer.setScissor(0, {scissor});
    mContext.commandBuffer.draw(3, 1, 0, 0);

    mContext.commandBuffer.endRenderPass();
    mContext.commandBuffer.end();
}

auto Application::drawFrame() -> void {
    if (auto result = mContext.device.waitForFences({mContext.inFlightFence}, vk::True, UINT64_MAX);
        result != vk::Result::eSuccess) {
        throw std::runtime_error(std::format("{}", vk::to_string(result)));
    }
    mContext.device.resetFences({mContext.inFlightFence});

    vk::AcquireNextImageInfoKHR acquireInfo{};
    acquireInfo.setSwapchain(mContext.swapchain);
    acquireInfo.setSemaphore(mContext.imageAvailableSemaphore);
    acquireInfo.setDeviceMask(1);
    acquireInfo.setTimeout(UINT64_MAX);

    auto result = mContext.device.acquireNextImage2KHR(acquireInfo);
    mContext.commandBuffer.reset();
    recordCommandBuffer(result.second);

    vk::SubmitInfo submitInfo{};
    vk::Semaphore waitSemaphores[] = {mContext.imageAvailableSemaphore};
    vk::PipelineStageFlags waitStages[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
    submitInfo.setWaitSemaphoreCount(1);
    submitInfo.setPWaitSemaphores(waitSemaphores);
    submitInfo.setPWaitDstStageMask(waitStages);
    submitInfo.setCommandBufferCount(1);
    submitInfo.setPCommandBuffers(&*(mContext.commandBuffer));

    vk::Semaphore signalSemaphores[] = {mContext.renderFinishedSemaphore};
    submitInfo.setSignalSemaphoreCount(1);
    submitInfo.setPSignalSemaphores(signalSemaphores);

    mContext.graphicsQueue.submit({submitInfo}, mContext.inFlightFence);

    vk::PresentInfoKHR presentInfo{};
    presentInfo.setWaitSemaphoreCount(1);
    presentInfo.setPWaitSemaphores(signalSemaphores);

    vk::SwapchainKHR swapChains[] = {mContext.swapchain};
    presentInfo.setSwapchainCount(1);
    presentInfo.setPSwapchains(swapChains);
    presentInfo.setPImageIndices(&result.second);
    presentInfo.setPResults(nullptr);

    if (auto result = mContext.presentQueue.presentKHR(presentInfo); result != vk::Result::eSuccess) {
        throw std::runtime_error(std::format("{}", vk::to_string(result)));
    }
}
