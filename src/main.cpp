#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <spdlog/spdlog.h>

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <stdexcept>
#include <sys/types.h>

// #define NDEBUG
#include "core/InstanceManager.h"

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

class HelloTriangleApplication
{
  public:
	void run()
	{
		initWindow();
		initVulkan();
		mainLoop();
		cleanup();
	}

  private:
	void initWindow()
	{
		if (glfwInit() == GLFW_FALSE)
		{
			throw std::runtime_error("Failed to initialize GLFW!");
		}

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		pWindow = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
		if (pWindow == NULL)
		{
			throw std::runtime_error("Failed to create window!");
		}
	}
	void initVulkan()
	{
		pInstanceManager = new InstanceManager();
		// pickPhysicalDevice();
	}
	void mainLoop()
	{
		while (!glfwWindowShouldClose(pWindow))
		{
			glfwPollEvents();
		}
	}
	void cleanup()
	{

		glfwDestroyWindow(pWindow);
		delete pInstanceManager;
		glfwTerminate();
	}

	// void pickPhysicalDevice()
	// {
	// 	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	// 	uint32_t deviceCount = 0;
	// 	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

	// 	if (deviceCount == 0)
	// 	{
	// 		throw std::runtime_error("failed to find GPUs with Vulkan support!");
	// 	}

	// 	std::vector<VkPhysicalDevice> devices(deviceCount);
	// 	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

	// 	std::multimap<int, VkPhysicalDevice> candidates;
	// 	for (const auto &device : devices)
	// 	{
	// 		int score = rateDeviceSuitability(device);
	// 		candidates.insert(std::make_pair(score, device));
	// 	}

	// 	if (candidates.rbegin()->first > 0)
	// 	{
	// 		physicalDevice = candidates.rbegin()->second;
	// 	}
	// 	else
	// 	{
	// 		throw std::runtime_error("failed to find a suitable GPU!");
	// 	}
	// }

	// int rateDeviceSuitability(VkPhysicalDevice device)
	// {
	// 	VkPhysicalDeviceProperties deviceProperties;
	// 	VkPhysicalDeviceFeatures deviceFeatures;
	// 	vkGetPhysicalDeviceProperties(device, &deviceProperties);
	// 	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

	// 	int score = 0;
	// 	if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
	// 	{
	// 		score += 1000;
	// 	}

	// 	score += deviceProperties.limits.maxImageDimension2D;

	// 	return score;
	// }

	GLFWwindow *pWindow = nullptr;
	InstanceManager *pInstanceManager = nullptr;
};

int main()
{
#ifndef NDEBUG
	spdlog::set_level(spdlog::level::debug);
#endif
	HelloTriangleApplication app;

	try
	{
		app.run();
	}
	catch (const std::exception &e)
	{
		spdlog::error(e.what());

		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}