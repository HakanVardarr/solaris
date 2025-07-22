#pragma once

#include <vulkan/vulkan.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <expected>
#include <format>
#include <string_view>

#define CONCAT(x, y) x##y
#define EXPECT_VOID(_T) std::expected<void, _T>
#define RETURN_ERROR(_FN)                                                   \
    if (auto CONCAT(_result_, __LINE__) = _FN; !CONCAT(_result_, __LINE__)) \
        return std::unexpected(CONCAT(_result_, __LINE__).error());

enum class ApplicationError : uint8_t {
    GLFWInitialization,
    WindowCreation,
};

template <>
struct std::formatter<ApplicationError> : std::formatter<std::string_view> {
    auto format(ApplicationError err, format_context& ctx) const {
        std::string_view msg;
        switch (err) {
            case ApplicationError::GLFWInitialization:
                msg = "Failed to initialize GLFW!";
                break;
            case ApplicationError::WindowCreation:
                msg = "Failed to create window.";
                break;
            default:
                msg = "Unknown error.";
                break;
        }
        return std::formatter<std::string_view>::format(msg, ctx);
    }
};

class Application {
   public:
    auto Run() -> EXPECT_VOID(ApplicationError);
    ~Application();

   private:
    auto initWindow() -> EXPECT_VOID(ApplicationError);
    auto initVulkan() -> EXPECT_VOID(ApplicationError);
    auto mainLoop() -> EXPECT_VOID(ApplicationError);

    GLFWwindow* pWindow = nullptr;
};
