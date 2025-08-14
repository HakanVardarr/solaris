#pragma once
#include "errors/application_error.hpp"
#include "errors/device_manager_error.hpp"
#include "errors/surface_manager_error.hpp"
#include "errors/vulkan_context_error.hpp"

#include <spdlog/fmt/bundled/format.h>

namespace fmt {

using namespace solaris::errors;

template <>
struct formatter<ApplicationError> {
    constexpr auto parse(format_parse_context& ctx) -> const char* { return ctx.end(); }

    template <typename FormatContext>
    auto format(const ApplicationError& err, FormatContext& ctx) const {
        std::string_view msg;
        switch (err.mErrorCode) {
            case ApplicationError::ErrorCode::eGLFWInitialization:
                msg = "Failed to initialize GLFW!";
                break;
            case ApplicationError::ErrorCode::eWindowCreation:
                msg = "Failed to create window.";
                break;
            case ApplicationError::ErrorCode::eVulkanContext:
                msg = err.mErrorMessage;
                break;
            default:
                msg = "Unknown error.";
                break;
        }
        return fmt::format_to(ctx.out(), "{}", msg);
    }
};

template <>
struct formatter<VulkanContextError> {
    constexpr auto parse(format_parse_context& ctx) -> const char* { return ctx.end(); }

    template <typename FormatContext>
    auto format(const VulkanContextError& err, FormatContext& ctx) const {
        std::string_view msg;

        switch (err.mErrorCode) {
            case VulkanContextError::ErrorCode::eEnumerateInstanceProperties:
                return fmt::format_to(ctx.out(), "Encountered error while enumerating instance layer properties: [{}]",
                                      err.mErrorMessage);
            case VulkanContextError::ErrorCode::eInstanceCreate:
                return fmt::format_to(ctx.out(), "Failed to create instance: [{}]", err.mErrorMessage);
                break;
            case VulkanContextError::ErrorCode::eDebugMessenger:
                return fmt::format_to(ctx.out(), "Failed to create debug messenger: [{}]", err.mErrorMessage);
            case VulkanContextError::ErrorCode::eDeviceManager:
                return fmt::format_to(ctx.out(), "{}", err.mErrorMessage);
            case VulkanContextError::ErrorCode::eSurfaceManager:
                return fmt::format_to(ctx.out(), "{}", err.mErrorMessage);
            default:
                return fmt::format_to(ctx.out(), "Unknown Error");
        }
    }
};

template <>
struct formatter<DeviceManagerError> {
    constexpr auto parse(format_parse_context& ctx) -> const char* { return ctx.end(); }

    template <typename FormatContext>
    auto format(const DeviceManagerError& err, FormatContext& ctx) const {
        std::string_view msg;

        switch (err.mErrorCode) {
            case DeviceManagerError::ErrorCode::eFailToFindGPU:
                return fmt::format_to(ctx.out(), "Failed to find GPUs with Vulkan support!");
            case DeviceManagerError::ErrorCode::eFailedToEnumerate:
                return fmt::format_to(ctx.out(), "Failed to enumerate physical devices: [{}]", err.mErrorMessage);
            case DeviceManagerError::ErrorCode::eFailedToCreateDevice:
                return fmt::format_to(ctx.out(), "Failed to create logical device: [{}]", err.mErrorMessage);
            case DeviceManagerError::ErrorCode::eFailedToGetQueue:
                return fmt::format_to(ctx.out(), "Failed to get queue from device: [{}]", err.mErrorMessage);
            default:
                return fmt::format_to(ctx.out(), "Unknown Error");
        }
    }
};

template <>
struct formatter<SurfaceManagerError> {
    constexpr auto parse(format_parse_context& ctx) -> const char* { return ctx.end(); }

    template <typename FormatContext>
    auto format(const SurfaceManagerError& err, FormatContext& ctx) const {
        std::string_view msg;

        switch (err.mErrorCode) {
            case SurfaceManagerError::ErrorCode::eFailedToCreateSurface:
                return fmt::format_to(ctx.out(), "Failed to create window surface!");

            default:
                return fmt::format_to(ctx.out(), "Unknown Error");
        }
    }
};

}  // namespace fmt
