#pragma once
#include "vulkan/vulkan_core.h"

extern const bool enableValidationLayers;

class DebugMessager
{
  public:
	DebugMessager(VkInstance *instance);
	~DebugMessager();

	static void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);

  private:
	VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pDebugMessenger);
	void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks *pAllocator);

	VkDebugUtilsMessengerEXT debugMessenger;
	VkInstance *pInstance;
};
