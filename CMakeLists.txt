cmake_minimum_required(VERSION 3.29)
project(solaris)

set(CMAKE_CXX_STANDARD 23)

find_package(Vulkan REQUIRED)
find_package(GLFW3 REQUIRED)
find_package(spdlog REQUIRED)


include_directories(
    ${PROJECT_SOURCE_DIR}/include   
)

set(SOURCES
    ${PROJECT_SOURCE_DIR}/src/core/DebugMessager.cpp
    ${PROJECT_SOURCE_DIR}/src/core/InstanceManager.cpp
)

set(MAIN
    ${PROJECT_SOURCE_DIR}/src/main.cpp
)

add_executable(${PROJECT_NAME} ${MAIN} ${SOURCES})
target_link_libraries(${PROJECT_NAME} Vulkan::Vulkan glfw spdlog::spdlog)