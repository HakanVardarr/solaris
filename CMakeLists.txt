cmake_minimum_required(VERSION 3.10)
project(solaris VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(Vulkan 1.4 REQUIRED)
find_package(glfw3 3.4 REQUIRED)
find_package(spdlog 1.15 REQUIRED)
find_package(glm REQUIRED)
find_package(VulkanMemoryAllocator CONFIG REQUIRED)
find_package(VulkanMemoryAllocator-Hpp CONFIG REQUIRED)

file(GLOB_RECURSE LIB_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

add_library(solaris SHARED ${LIB_SOURCES})

target_include_directories(solaris
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include> 
)

target_link_libraries(solaris
    PUBLIC  Vulkan::Vulkan 
            glm::glm 
            spdlog::spdlog
            glfw GPUOpen::VulkanMemoryAllocator
            VulkanMemoryAllocator-Hpp::VulkanMemoryAllocator-Hpp
)

target_compile_definitions(solaris PRIVATE
    $<$<CONFIG:Release>:NDEBUG> GLFW_INCLUDE_VULKAN
)

add_subdirectory(sandbox)
