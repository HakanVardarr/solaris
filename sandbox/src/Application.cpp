#include <Solaris/Graphics/Vulkan/Shader.hpp>

#include "Application.hpp"
#include "Vertex.hpp"

// clang-format off
const std::vector<Vertex> vertices = {
    {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
    {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
};

const std::vector<uint16_t> indices = {
    0, 1, 2, 2, 3, 0
};
// clang-format on

void TriangleApplication::onInit() {
    mVertexBuffer.init(*ctx().allocator, vertices, ctx().commandPool, ctx().device, ctx().graphicsQueue);
    mIndexBuffer.init(*ctx().allocator, indices, ctx().commandPool, ctx().device, ctx().graphicsQueue);
    createPipeline();
}

void TriangleApplication::onRender(vk::raii::CommandBuffer& cmd, uint32_t imageIndex) {
    cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, *mPipeline);

    vk::Buffer vbufs[] = {mVertexBuffer.getBuffer()};
    vk::DeviceSize offs[] = {0};

    cmd.bindVertexBuffers(0, vbufs, offs);
    cmd.bindIndexBuffer(mIndexBuffer.getBuffer(), 0, vk::IndexType::eUint16);

    vk::Viewport viewport{};
    viewport.setX(0.0f);
    viewport.setY(0.0f);
    viewport.setWidth(static_cast<float>(ctx().swapchainExtent.width));
    viewport.setHeight(static_cast<float>(ctx().swapchainExtent.height));
    viewport.setMinDepth(0.0f);
    viewport.setMaxDepth(1.0f);

    cmd.setViewport(0, {viewport});

    vk::Rect2D scissor{};
    scissor.setOffset({0, 0});
    scissor.setExtent(ctx().swapchainExtent);

    cmd.setScissor(0, {scissor});
    cmd.drawIndexed(static_cast<uint32_t>(mIndexBuffer.getIndexCount()), 1, 0, 0, 0);
}

void TriangleApplication::createPipeline() {
    auto vertCode = Solaris::Graphics::Vulkan::readFile("shaders/shader.vert.spv");
    auto fragCode = Solaris::Graphics::Vulkan::readFile("shaders/shader.frag.spv");

    auto vertShader = Solaris::Graphics::Vulkan::createShaderModule(ctx().device, vertCode);
    auto fragShader = Solaris::Graphics::Vulkan::createShaderModule(ctx().device, fragCode);

    vk::PipelineShaderStageCreateInfo vsi{};
    vsi.setStage(vk::ShaderStageFlagBits::eVertex);
    vsi.setModule(vertShader);
    vsi.setPName("main");

    vk::PipelineShaderStageCreateInfo fsi{};
    fsi.setStage(vk::ShaderStageFlagBits::eFragment);
    fsi.setModule(fragShader);
    fsi.setPName("main");

    vk::PipelineShaderStageCreateInfo shaderStages[] = {vsi, fsi};
    std::vector<vk::DynamicState> dynamicStates = {
        vk::DynamicState::eViewport,
        vk::DynamicState::eScissor,
    };

    vk::PipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();

    vk::PipelineVertexInputStateCreateInfo vertexInputInfo{};

    auto bindingDescription = Vertex::getBindingDescription();
    auto attributeDescriptions = Vertex::getAttributeDescriptions();

    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

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

    mPipelineLayout = {ctx().device, pipelineLayoutInfo};

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
    pipelineInfo.setLayout(mPipelineLayout);
    pipelineInfo.setRenderPass(ctx().renderPass);
    pipelineInfo.setSubpass(0);
    pipelineInfo.setBasePipelineHandle(VK_NULL_HANDLE);
    pipelineInfo.setBasePipelineIndex(-1);

    mPipeline = ctx().device.createGraphicsPipeline(nullptr, pipelineInfo);
}