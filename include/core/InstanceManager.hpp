#pragma once
#include "Macros.hpp"
#include "errors/InsanceManagerError.hpp"

#include <vulkan/vulkan.hpp>

namespace solaris::core {

using solaris::errors::InstanceManagerError;

class InstanceManager {
   public:
    InstanceManager(){};
    ~InstanceManager() { mInstance.destroy(); };

    auto CreateInstance() -> EXPECT_VOID(InstanceManagerError);
    auto GetInstance() -> vk::Instance const { return mInstance; }

   private:
    vk::Instance mInstance;
};
}  // namespace solaris::core
