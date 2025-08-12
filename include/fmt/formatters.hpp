#pragma once
#include "errors/ApplicationError.hpp"
#include "errors/InsanceManagerError.hpp"

#include <spdlog/fmt/bundled/format.h>

#include <format>

namespace fmt {

using namespace solaris::errors;

template <>
struct formatter<InstanceManagerError> {
    constexpr auto parse(fmt::format_parse_context& ctx) -> const char* { return ctx.end(); }

    template <typename FormatContext>
    auto format(const InstanceManagerError& err, FormatContext& ctx) const {
        std::string_view msg;
        switch (err.mErrorCode) {
            case InstanceManagerError::ErrorCode::eFailedToCreateInstance:
                msg = "Failed to create Vulkan instance!";
                break;
            case InstanceManagerError::ErrorCode::eEnumerateExtensions:
                msg = "Encountered error while enumerating extensions.";
                break;
            case InstanceManagerError::ErrorCode::eInstanceLayerProperties:
                msg = "Encountered error while enumerating instance layer properties.";
                break;
            case InstanceManagerError::ErrorCode::eRequiredExtensionDoesNotExists:
                msg = "Required extension does not exists.";
                break;
            case InstanceManagerError::ErrorCode::eValidationLayersNotAvailable:
                msg = "Validation layers requested, but not available.";
                break;
            default:
                msg = "Unknown error.";
                break;
        }

        std::string full = std::format("{} ({})", msg, err.mErrorMessage);
        return fmt::format_to(ctx.out(), "{}", full);
    }
};

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
            case ApplicationError::ErrorCode::eInstanceManagerError:
                msg = err.mErrorMessage;
                break;
            default:
                msg = "Unknown error.";
                break;
        }
        return fmt::format_to(ctx.out(), "{}", msg);
    }
};

}  // namespace fmt
