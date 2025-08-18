#pragma once
#include "Graphics/Vulkan/Context.hpp"

#include <GLFW/glfw3.h>

class Application {
   public:
    auto Run() -> void;
    ~Application();

   private:
    void initWindow();
    void initVulkan();
    void mainLoop();
    void recordCommandBuffer(uint32_t imageIndex);
    void drawFrame();

    GLFWwindow* pWindow = nullptr;
    Solaris::Graphics::Vulkan::Context mContext;
};
