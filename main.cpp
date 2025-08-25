#include "Core/Application.hpp"
#include "Graphics/Vulkan/Shader.hpp"
#include "include/Graphics/Vulkan/Buffer.hpp"

// #include <vulkan/vulkan.hpp>
// #include <vulkan/vulkan_raii.hpp>

#include <spdlog/common.h>
#include <spdlog/spdlog.h>
#include <glm/glm.hpp>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_raii.hpp>
#include <vulkan/vulkan_structs.hpp>

#include <stdexcept>

struct Vertex {
    glm::vec2 pos;
    glm::vec3 color;

    static vk::VertexInputBindingDescription getBindingDescription() {
        vk::VertexInputBindingDescription bindingDescription{};
        bindingDescription.setBinding(0);
        bindingDescription.setStride(sizeof(Vertex));
        bindingDescription.setInputRate(vk::VertexInputRate::eVertex);

        return bindingDescription;
    }
    static std::array<vk::VertexInputAttributeDescription, 2> getAttributeDescriptions() {
        std::array<vk::VertexInputAttributeDescription, 2> attributeDescriptions{};

        attributeDescriptions[0].setBinding(0);
        attributeDescriptions[0].setLocation(0);
        attributeDescriptions[0].setFormat(vk::Format::eR32G32Sfloat);
        attributeDescriptions[0].setOffset(offsetof(Vertex, pos));

        attributeDescriptions[1].setBinding(0);
        attributeDescriptions[1].setLocation(1);
        attributeDescriptions[1].setFormat(vk::Format::eR32G32B32Sfloat);
        attributeDescriptions[1].setOffset(offsetof(Vertex, color));

        return attributeDescriptions;
    }
};

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

class TriangleApplication final : public Application {
   public:
    TriangleApplication(){};
    ~TriangleApplication() = default;

   protected:
    void onInit() override {
        mVertexBuffer.init(*ctx().allocator, vertices, ctx().commandPool, ctx().device, ctx().graphicsQueue);
        mIndexBuffer.init(*ctx().allocator, indices, ctx().commandPool, ctx().device, ctx().graphicsQueue);
        createPipeline();
    }
    void onRender(vk::raii::CommandBuffer& cmd, uint32_t imageIndex) override {
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
    void onShutdown() override {}

   private:
    void createPipeline() {
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

    Solaris::Graphics::Vulkan::VertexBuffer mVertexBuffer;
    Solaris::Graphics::Vulkan::IndexBuffer mIndexBuffer;

    vk::raii::PipelineLayout mPipelineLayout{nullptr};
    vk::raii::Pipeline mPipeline{nullptr};
};

auto main(int argc, char** argv) -> int {
    try {
        TriangleApplication app;
        app.Run();

    } catch (std::runtime_error& err) {
        spdlog::error("{}", err.what());
    }
    return EXIT_SUCCESS;
}
