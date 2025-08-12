#pragma once
#include "Macros.hpp"
#include "core/InstanceManager.hpp"
#include "errors/ApplicationError.hpp"

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
    InstanceManager mInstanceManager;
};

}  // namespace solaris::core
