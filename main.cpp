#include "Application.hpp"
#include "Macros.hpp"

#include <expected>

#include <spdlog/common.h>
#include <spdlog/spdlog.h>

auto main(int argc, char** argv) -> int {
    spdlog::set_level(spdlog::level::debug);

    Application app;
    if (auto result = app.Run(); !result) {
        spdlog::error("{}", FORMAT_ERROR(result));
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
