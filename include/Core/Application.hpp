#pragma once
#include "Graphics/Vulkan/Context.hpp"

#include <GLFW/glfw3.h>
#include <vulkan/vulkan_raii.hpp>

class Application {
   public:
    void Run();
    virtual ~Application();

    bool mFramebufferResized = false;

   protected:
    virtual void onInit(){};
    virtual void onUpdate(float dt){};
    virtual void onRender(vk::raii::CommandBuffer& cmd, uint32_t imageIndex){};
    virtual void onShutdown(){};

    GLFWwindow* window() const { return pWindow; }
    Solaris::Graphics::Vulkan::Context& ctx() { return mContext; }
    const Solaris::Graphics::Vulkan::Context& ctx() const { return mContext; }
    const std::chrono::steady_clock::time_point lastTick() const { return mLastTick; }

   private:
    void initLogger();
    void initWindow();
    void initVulkan();
    void mainLoop();
    void recordCommandBuffer(const vk::raii::CommandBuffer& commandBuffer, uint32_t imageIndex);
    void drawFrame();

    GLFWwindow* pWindow = nullptr;
    Solaris::Graphics::Vulkan::Context mContext;
    std::chrono::steady_clock::time_point mLastTick{};
};
