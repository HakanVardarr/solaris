#include "Core/Application.hpp"
#include "Graphics/Vulkan/Context.hpp"

#include <spdlog/fmt/bundled/format.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include <vulkan/vk_enum_string_helper.h>
#include <vulkan/vulkan_core.h>
#include <chrono>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_raii.hpp>
#include <vulkan/vulkan_to_string.hpp>

#include <cstdint>
#include <stdexcept>

Application::~Application() {
    mContext.device.waitIdle();
    onShutdown();
}

void Application::Run() {
    initLogger();
    initWindow();
    initVulkan();

    onInit();
    mLastTick = std::chrono::steady_clock::now();
    mainLoop();
}

void Application::initLogger() {
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_pattern("[%T] [%^%l%$] %v");

    auto logger = std::make_shared<spdlog::logger>("Solaris", console_sink);
    spdlog::set_default_logger(logger);

#if defined(NDEBUG)
    spdlog::set_level(spdlog::level::warn);
#else
    spdlog::set_level(spdlog::level::debug);
#endif
}

static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
    auto app = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
    app->mFramebufferResized = true;
}

void Application::initWindow() {
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
    glfwSetWindowUserPointer(pWindow, this);
    glfwSetFramebufferSizeCallback(pWindow, framebufferResizeCallback);
}

void Application::initVulkan() {
    try {
        mContext.init(pWindow);
    } catch (vk::SystemError& err) {
        throw std::runtime_error(err.what());
    }
}

void Application::mainLoop() {
    while (glfwWindowShouldClose(pWindow) == GLFW_FALSE) {
        glfwPollEvents();
        auto now = std::chrono::steady_clock::now();
        float dt = std::chrono::duration<float>(now - mLastTick).count();
        mLastTick = now;

        onUpdate(dt);
        drawFrame();
    }

    mContext.device.waitIdle();
}

void Application::recordCommandBuffer(const vk::raii::CommandBuffer& commandBuffer, uint32_t imageIndex) {
    vk::CommandBufferBeginInfo beginInfo{};
    beginInfo.setPInheritanceInfo(nullptr);

    commandBuffer.begin(beginInfo);

    vk::RenderPassBeginInfo renderPassInfo{};
    renderPassInfo.setRenderPass(mContext.renderPass);
    renderPassInfo.setFramebuffer(mContext.swapchainFramebuffers[imageIndex]);
    renderPassInfo.renderArea.setOffset({0, 0});
    renderPassInfo.renderArea.setExtent(mContext.swapchainExtent);

    vk::ClearValue clearColor;
    clearColor.color = vk::ClearColorValue(std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f});
    renderPassInfo.setClearValueCount(1);
    renderPassInfo.setPClearValues(&clearColor);

    commandBuffer.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
    onRender(const_cast<vk::raii::CommandBuffer&>(commandBuffer), imageIndex);

    commandBuffer.endRenderPass();
    commandBuffer.end();
}

void Application::drawFrame() {
    auto& frame = mContext.frames.getCurrentFrame();

    if (auto result = mContext.device.waitForFences({frame.inFlightFence}, vk::True, UINT64_MAX);
        result != vk::Result::eSuccess) {
        throw std::runtime_error(std::format("{}", vk::to_string(result)));
    }
    mContext.device.resetFences({frame.inFlightFence});

    vk::AcquireNextImageInfoKHR acquireInfo{};
    acquireInfo.setSwapchain(mContext.swapchain);
    acquireInfo.setSemaphore(frame.imageAvailableSemaphore);
    acquireInfo.setDeviceMask(1);
    acquireInfo.setTimeout(UINT64_MAX);

    auto acquireRes = mContext.device.acquireNextImage2KHR(acquireInfo);
    uint32_t imageIndex = acquireRes.second;

    frame.commandBuffer.reset();
    recordCommandBuffer(frame.commandBuffer, imageIndex);

    vk::SubmitInfo submitInfo{};
    vk::Semaphore waitSemaphores[] = {frame.imageAvailableSemaphore};
    vk::PipelineStageFlags waitStages[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
    submitInfo.setWaitSemaphoreCount(1);
    submitInfo.setPWaitSemaphores(waitSemaphores);
    submitInfo.setPWaitDstStageMask(waitStages);
    submitInfo.setCommandBufferCount(1);
    submitInfo.setPCommandBuffers(&*frame.commandBuffer);

    vk::Semaphore signalSemaphores[] = {frame.renderFinishedSemaphore};
    submitInfo.setSignalSemaphoreCount(1);
    submitInfo.setPSignalSemaphores(signalSemaphores);

    mContext.graphicsQueue.submit({submitInfo}, frame.inFlightFence);

    vk::PresentInfoKHR presentInfo{};
    presentInfo.setWaitSemaphoreCount(1);
    presentInfo.setPWaitSemaphores(signalSemaphores);
    vk::SwapchainKHR swapChains[] = {mContext.swapchain};
    presentInfo.setSwapchainCount(1);
    presentInfo.setPSwapchains(swapChains);
    presentInfo.setPImageIndices(&imageIndex);

    if (auto result = mContext.presentQueue.presentKHR(presentInfo);
        result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR || mFramebufferResized) {
        int width = 0, height = 0;
        glfwGetFramebufferSize(pWindow, &width, &height);
        while (width == 0 || height == 0) {
            glfwGetFramebufferSize(pWindow, &width, &height);
            glfwWaitEvents();
        }
        mContext.recreateSwapchain(pWindow);
        mFramebufferResized = false;
    }

    mContext.frames.updateFrame();
}

//     vk::CommandBufferBeginInfo beginInfo{};
//     beginInfo.setPInheritanceInfo(nullptr);

//     commandBuffer.begin(beginInfo);

//     vk::RenderPassBeginInfo renderPassInfo{};
//     renderPassInfo.setRenderPass(mContext.renderPass);
//     renderPassInfo.setFramebuffer(mContext.swapchainFramebuffers[imageIndex]);
//     renderPassInfo.renderArea.setOffset({0, 0});
//     renderPassInfo.renderArea.setExtent(mContext.swapchainExtent);

//     vk::ClearValue clearColor;
//     clearColor.color = vk::ClearColorValue(std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f});
//     renderPassInfo.setClearValueCount(1);
//     renderPassInfo.setPClearValues(&clearColor);

//     commandBuffer.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
//     commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, mContext.pipeline);

//     vk::Buffer vertexBuffers[] = {mContext.vertexBuffer.getBuffer()};
//     vk::DeviceSize offsets[] = {0};

//     commandBuffer.bindVertexBuffers(0, vertexBuffers, offsets);
//     commandBuffer.bindIndexBuffer(mContext.indexBuffer.getBuffer(), 0, vk::IndexType::eUint16);

//     vk::Viewport viewport{};
//     viewport.setX(0.0f);
//     viewport.setY(0.0f);
//     viewport.setWidth(static_cast<float>(mContext.swapchainExtent.width));
//     viewport.setHeight(static_cast<float>(mContext.swapchainExtent.height));
//     viewport.setMinDepth(0.0f);
//     viewport.setMaxDepth(1.0f);

//     commandBuffer.setViewport(0, {viewport});

//     vk::Rect2D scissor{};
//     scissor.setOffset({0, 0});
//     scissor.setExtent(mContext.swapchainExtent);

//     commandBuffer.setScissor(0, {scissor});
//     commandBuffer.drawIndexed(static_cast<uint32_t>(mContext.indexBuffer.getIndexCount()), 1, 0, 0, 0);

//     commandBuffer.endRenderPass();
//     commandBuffer.end();
// }

// void Application::drawFrame() {
//     auto& frame = mContext.frames.getCurrentFrame();

//     if (auto result = mContext.device.waitForFences({frame.inFlightFence}, vk::True, UINT64_MAX);
//         result != vk::Result::eSuccess) {
//         throw std::runtime_error(std::format("{}", vk::to_string(result)));
//     }
//     mContext.device.resetFences({frame.inFlightFence});

//     vk::AcquireNextImageInfoKHR acquireInfo{};
//     acquireInfo.setSwapchain(mContext.swapchain);
//     acquireInfo.setSemaphore(frame.imageAvailableSemaphore);
//     acquireInfo.setDeviceMask(1);
//     acquireInfo.setTimeout(UINT64_MAX);

//     auto result = mContext.device.acquireNextImage2KHR(acquireInfo);
//     uint32_t imageIndex = result.second;

//     frame.commandBuffer.reset();
//     recordCommandBuffer(frame.commandBuffer, imageIndex);

//     vk::SubmitInfo submitInfo{};
//     vk::Semaphore waitSemaphores[] = {frame.imageAvailableSemaphore};
//     vk::PipelineStageFlags waitStages[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
//     submitInfo.setWaitSemaphoreCount(1);
//     submitInfo.setPWaitSemaphores(waitSemaphores);
//     submitInfo.setPWaitDstStageMask(waitStages);
//     submitInfo.setCommandBufferCount(1);
//     submitInfo.setPCommandBuffers(&*frame.commandBuffer);

//     vk::Semaphore signalSemaphores[] = {frame.renderFinishedSemaphore};
//     submitInfo.setSignalSemaphoreCount(1);
//     submitInfo.setPSignalSemaphores(signalSemaphores);

//     mContext.graphicsQueue.submit({submitInfo}, frame.inFlightFence);

//     vk::PresentInfoKHR presentInfo{};
//     presentInfo.setWaitSemaphoreCount(1);
//     presentInfo.setPWaitSemaphores(signalSemaphores);

//     vk::SwapchainKHR swapChains[] = {mContext.swapchain};
//     presentInfo.setSwapchainCount(1);
//     presentInfo.setPSwapchains(swapChains);
//     presentInfo.setPImageIndices(&imageIndex);
//     presentInfo.setPResults(nullptr);

//     if (auto result = mContext.presentQueue.presentKHR(presentInfo);
//         result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR || mFramebufferResized) {
//         int width = 0, height = 0;
//         glfwGetFramebufferSize(pWindow, &width, &height);
//         while (width == 0 || height == 0) {
//             glfwGetFramebufferSize(pWindow, &width, &height);
//             glfwWaitEvents();
//         }

//         mContext.recreateSwapchain(pWindow);
//         mFramebufferResized = false;
//     }

//     mContext.frames.updateFrame();
// }
