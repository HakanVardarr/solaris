#include "../include/Application.hpp"

#include <expected>
#include <format>
#include <print>

auto main(int argc, char** argv) -> int {
    Application app;
    if (auto result = app.Run(); !result) {
        std::println("ERROR: {}", result.error());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
