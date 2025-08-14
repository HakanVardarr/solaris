cmake_minimum_required(VERSION 3.10)
project(solaris VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)
find_package(spdlog REQUIRED)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    src/*.cpp
    main.cpp
)

add_executable(solaris ${SOURCES})

target_include_directories(solaris
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(solaris
    PRIVATE
        glfw
        spdlog::spdlog
        Vulkan::Vulkan
)

target_compile_definitions(solaris PRIVATE
    $<$<CONFIG:Release>:NDEBUG>
    GLFW_INCLUDE_VULKAN
)
