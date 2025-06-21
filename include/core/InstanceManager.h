#pragma once

#include "core/DebugMessager.h"
#include <vulkan/vulkan_core.h>

class InstanceManager {
public:
  InstanceManager();
  ~InstanceManager();

  VkInstance getInstance() const noexcept { return instance; }

private:
  VkInstance instance;
  DebugMessager *pDebugMessenger;
};