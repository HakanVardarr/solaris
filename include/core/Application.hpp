#pragma once
#include "core/vulkan_context.hpp"
#include "errors/application_error.hpp"
#include "macros.hpp"

#include <GLFW/glfw3.h>

namespace solaris::core {

class Application {
   public:
    auto Run() -> EXPECT_VOID(solaris::errors::ApplicationError);
    ~Application();

   private:
    auto initWindow() -> EXPECT_VOID(solaris::errors::ApplicationError);
    auto initVulkan() -> EXPECT_VOID(solaris::errors::ApplicationError);
    auto mainLoop() -> EXPECT_VOID(solaris::errors::ApplicationError);

    GLFWwindow* pWindow = nullptr;
    VulkanContext mContext;
    // InstanceManager mInstanceManager;
};

}  // namespace solaris::core
