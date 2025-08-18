#include "Graphics/Vulkan/Pipeline.hpp"
#include <vulkan/vulkan_core.h>

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_raii.hpp>

#include <cstddef>
#include <cstdint>
#include <fstream>
#include <stdexcept>

namespace Solaris::Graphics::Vulkan {

auto readFile(const std::string& fileName) -> std::vector<char> {
    std::ifstream file(fileName, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error(std::format("Failed to open file {}", fileName));
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);
    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();
    return buffer;
}

auto createShaderModule(VulkanContext& ctx, const std::vector<char>& code) -> vk::raii::ShaderModule {
    vk::ShaderModuleCreateInfo createInfo({}, code.size(), reinterpret_cast<const uint32_t*>(code.data()));

    return vk::raii::ShaderModule(ctx.mDevice, createInfo);
}

auto CreateGraphicsPipeline(VulkanContext& ctx) -> void {
    auto vertShaderCode = readFile("shaders/shader.vert.spv");
    auto fragShaderCode = readFile("shaders/shader.frag.spv");

    vk::raii::ShaderModule vertShaderModule = createShaderModule(ctx, vertShaderCode);
    vk::raii::ShaderModule fragShaderModule = createShaderModule(ctx, fragShaderCode);

    vk::PipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.setStage(vk::ShaderStageFlagBits::eVertex);
    vertShaderStageInfo.setModule(vertShaderModule);
    vertShaderStageInfo.setPName("main");

    vk::PipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.setStage(vk::ShaderStageFlagBits::eFragment);
    fragShaderStageInfo.setModule(fragShaderModule);
    fragShaderStageInfo.setPName("main");

    vk::PipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

    std::vector<vk::DynamicState> dynamicStates = {
        vk::DynamicState::eViewport,
        vk::DynamicState::eScissor,
    };

    vk::PipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();

    vk::PipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.vertexBindingDescriptionCount = 0;
    vertexInputInfo.pVertexBindingDescriptions = nullptr;
    vertexInputInfo.vertexAttributeDescriptionCount = 0;
    vertexInputInfo.pVertexAttributeDescriptions = nullptr;

    vk::PipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.topology = vk::PrimitiveTopology::eTriangleList;
    inputAssembly.primitiveRestartEnable = vk::False;

    vk::PipelineViewportStateCreateInfo viewportState{};
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    vk::PipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.depthClampEnable = vk::False;
    rasterizer.rasterizerDiscardEnable = vk::False;
    rasterizer.polygonMode = vk::PolygonMode::eFill;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = vk::CullModeFlagBits::eBack;
    rasterizer.frontFace = vk::FrontFace::eClockwise;
    rasterizer.depthBiasEnable = vk::False;

    vk::PipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sampleShadingEnable = vk::False;
    multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;

    vk::PipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                                          vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
    colorBlendAttachment.blendEnable = vk::False;
    colorBlendAttachment.srcColorBlendFactor = vk::BlendFactor::eOne;
    colorBlendAttachment.dstColorBlendFactor = vk::BlendFactor::eZero;
    colorBlendAttachment.colorBlendOp = vk::BlendOp::eAdd;
    colorBlendAttachment.srcAlphaBlendFactor = vk::BlendFactor::eOne;
    colorBlendAttachment.dstAlphaBlendFactor = vk::BlendFactor::eZero;
    colorBlendAttachment.alphaBlendOp = vk::BlendOp::eAdd;

    vk::PipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.logicOpEnable = vk::False;
    colorBlending.logicOp = vk::LogicOp::eCopy;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    vk::PipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pSetLayouts = nullptr;
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges = nullptr;

    ctx.mPipelineLayout = {ctx.mDevice, pipelineLayoutInfo};

    vk::GraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.setStageCount(2);
    pipelineInfo.setPStages(shaderStages);
    pipelineInfo.setPVertexInputState(&vertexInputInfo);
    pipelineInfo.setPInputAssemblyState(&inputAssembly);
    pipelineInfo.setPViewportState(&viewportState);
    pipelineInfo.setPRasterizationState(&rasterizer);
    pipelineInfo.setPMultisampleState(&multisampling);
    pipelineInfo.setPDepthStencilState(nullptr);
    pipelineInfo.setPColorBlendState(&colorBlending);
    pipelineInfo.setPDynamicState(&dynamicState);
    pipelineInfo.setLayout(ctx.mPipelineLayout);
    pipelineInfo.setRenderPass(ctx.mRenderPass);
    pipelineInfo.setSubpass(0);
    pipelineInfo.setBasePipelineHandle(VK_NULL_HANDLE);
    pipelineInfo.setBasePipelineIndex(-1);

    ctx.mPipeline = ctx.mDevice.createGraphicsPipeline(nullptr, pipelineInfo);
}
}  // namespace Solaris::Graphics::Vulkan