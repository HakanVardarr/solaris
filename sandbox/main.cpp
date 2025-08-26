#include "Application.hpp"

#include <spdlog/spdlog.h>

int main() {
    try {
        TriangleApplication app;
        app.Run();

    } catch (std::runtime_error& err) {
        spdlog::error("{}", err.what());
    }
    return EXIT_SUCCESS;
}