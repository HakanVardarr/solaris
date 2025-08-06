#include "Application.hpp"

#include <spdlog/common.h>
#include <spdlog/spdlog.h>

auto main(int argc, char** argv) -> int {
#ifndef NDEBUG
    spdlog::set_level(spdlog::level::debug);
#else
    spdlog::set_level(spdlog::level::info);
#endif

    Application app;
    if (auto result = app.Run(); !result) {
        spdlog::error("{}", result.error());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
