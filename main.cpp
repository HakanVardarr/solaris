#include "Application.hpp"

#include <spdlog/common.h>
#include <spdlog/spdlog.h>
#include <stdexcept>

auto main(int argc, char** argv) -> int {
#ifndef NDEBUG
    spdlog::set_level(spdlog::level::debug);
#else
    spdlog::set_level(spdlog::level::info);
#endif

    try {
        Application app;
        app.Run();

    } catch (std::runtime_error& err) {
        spdlog::error("{}", err.what());
    }

    return EXIT_SUCCESS;
}
