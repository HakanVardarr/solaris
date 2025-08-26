#include <Solaris/Core/Application.hpp>
#include <Solaris/Graphics/Vulkan/Buffer.hpp>

#include <cstdint>
#include <vulkan/vulkan_raii.hpp>

class TriangleApplication : public Application {
   public:
    TriangleApplication(){};
    ~TriangleApplication() = default;

   protected:
    void onInit() override;
    void onRender(vk::raii::CommandBuffer& cmd, uint32_t imageIndex) override;
    void onShutdown() override{};

   private:
    void createPipeline();

    Solaris::Graphics::Vulkan::VertexBuffer mVertexBuffer;
    Solaris::Graphics::Vulkan::IndexBuffer mIndexBuffer;

    vk::raii::PipelineLayout mPipelineLayout{nullptr};
    vk::raii::Pipeline mPipeline{nullptr};
};