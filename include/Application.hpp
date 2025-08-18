#pragma once
#include "Graphics/Vulkan/Context.hpp"

#include <GLFW/glfw3.h>

class Application {
   public:
    auto Run() -> void;
    ~Application();

   private:
    auto initWindow() -> void;
    auto initVulkan() -> void;
    auto mainLoop() -> void;
    auto recordCommandBuffer(uint32_t imageIndex) -> void;
    auto drawFrame() -> void;

    GLFWwindow* pWindow = nullptr;
    Solaris::Graphics::Vulkan::VulkanContext mContext;
};
