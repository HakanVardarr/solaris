#include "Graphics/Vulkan/CommandBuffer.hpp"
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_raii.hpp>
#include "Graphics/Vulkan/Context.hpp"

namespace Solaris::Graphics::Vulkan {

auto CreateCommandBuffer(VulkanContext& ctx) -> void {
    vk::CommandBufferAllocateInfo allocInfo{};
    allocInfo.setCommandPool(ctx.mCommandPool);
    allocInfo.setLevel(vk::CommandBufferLevel::ePrimary);
    allocInfo.setCommandBufferCount(1);

    auto buffers = ctx.mDevice.allocateCommandBuffers(allocInfo);
    ctx.mCommandBuffer.swap(buffers[0]);
}

auto RecordCommandBuffer(VulkanContext& ctx, uint32_t imageIndex) -> void {
    vk::CommandBufferBeginInfo beginInfo{};
    beginInfo.setPInheritanceInfo(nullptr);

    ctx.mCommandBuffer.begin(beginInfo);

    vk::RenderPassBeginInfo renderPassInfo{};
    renderPassInfo.setRenderPass(ctx.mRenderPass);
    renderPassInfo.setFramebuffer(ctx.mSwapChainFramebuffers[imageIndex]);
    renderPassInfo.renderArea.setOffset({0, 0});
    renderPassInfo.renderArea.setExtent(ctx.mSwapChainExtent);

    vk::ClearValue clearColor;
    clearColor.color = vk::ClearColorValue(std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f});
    renderPassInfo.setClearValueCount(1);
    renderPassInfo.setPClearValues(&clearColor);

    ctx.mCommandBuffer.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
    ctx.mCommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, ctx.mPipeline);

    vk::Viewport viewport{};
    viewport.setX(0.0f);
    viewport.setY(0.0f);
    viewport.setWidth(static_cast<float>(ctx.mSwapChainExtent.width));
    viewport.setHeight(static_cast<float>(ctx.mSwapChainExtent.height));
    viewport.setMinDepth(0.0f);
    viewport.setMaxDepth(1.0f);

    ctx.mCommandBuffer.setViewport(0, {viewport});

    vk::Rect2D scissor{};
    scissor.setOffset({0, 0});
    scissor.setExtent(ctx.mSwapChainExtent);

    ctx.mCommandBuffer.setScissor(0, {scissor});
    ctx.mCommandBuffer.draw(3, 1, 0, 0);

    ctx.mCommandBuffer.endRenderPass();
}

}  // namespace Solaris::Graphics::Vulkan