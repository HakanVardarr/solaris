#include "Application.hpp"

#include <spdlog/common.h>
#include <spdlog/spdlog.h>
#include <stdexcept>

auto main(int argc, char** argv) -> int {
    try {
        Application app;
        app.Run();

    } catch (std::runtime_error& err) {
        spdlog::error("{}", err.what());
    }
    return EXIT_SUCCESS;
}
