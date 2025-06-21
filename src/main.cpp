#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <spdlog/spdlog.h>

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <stdexcept>
#include <sys/types.h>

// #define NDEBUG
#include "core/InstanceManager.h"

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

class HelloTriangleApplication {
public:
  void run() {
    initWindow();
    initVulkan();
    mainLoop();
    cleanup();
  }

private:
  void initWindow() {
    if (glfwInit() == GLFW_FALSE) {
      throw std::runtime_error("Failed to initialize GLFW!");
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    pWindow = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
    if (pWindow == NULL) {
      throw std::runtime_error("Failed to create window!");
    }
  }
  void initVulkan() { pInstanceManager = new InstanceManager(); }
  void mainLoop() {
    while (!glfwWindowShouldClose(pWindow)) {
      glfwPollEvents();
    }
  }
  void cleanup() {

    glfwDestroyWindow(pWindow);
    delete pInstanceManager;
    glfwTerminate();
  }

  GLFWwindow *pWindow = nullptr;
  InstanceManager *pInstanceManager = nullptr;
};

int main() {
#ifndef NDEBUG
  spdlog::set_level(spdlog::level::debug);
#endif
  HelloTriangleApplication app;

  try {
    app.run();
  } catch (const std::exception &e) {
    spdlog::error(e.what());

    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}