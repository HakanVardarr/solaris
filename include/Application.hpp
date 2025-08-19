#pragma once
#include "Graphics/Vulkan/Context.hpp"

#include <GLFW/glfw3.h>
#include <vulkan/vulkan_raii.hpp>

class Application {
   public:
    void Run();
    ~Application();

    bool mFramebufferResized = false;

   private:
    void initLogger();
    void initWindow();
    void initVulkan();
    void mainLoop();
    void recordCommandBuffer(const vk::raii::CommandBuffer& commandBuffer, uint32_t imageIndex);
    void drawFrame();

    GLFWwindow* pWindow = nullptr;
    Solaris::Graphics::Vulkan::Context mContext;
};
