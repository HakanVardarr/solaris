cmake_minimum_required(VERSION 3.10)
project(solaris VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Vulkan 1.4 REQUIRED)
find_package(glfw3 3.4 REQUIRED)
find_package(spdlog 1.15 REQUIRED)
find_package(glm REQUIRED)
find_package(VulkanMemoryAllocator CONFIG REQUIRED)
find_package(VulkanMemoryAllocator-Hpp CONFIG REQUIRED)

find_program(GLSLC glslc
    HINTS ENV VULKAN_SDK
    PATH_SUFFIXES bin
)
if(NOT GLSLC)
    message(FATAL_ERROR "failed to find glslc.")
endif()

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    ${CMAKE_SOURCE_DIR}/src/*.cpp
    ${CMAKE_SOURCE_DIR}/main.cpp
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
        GPUOpen::VulkanMemoryAllocator
        VulkanMemoryAllocator-Hpp::VulkanMemoryAllocator-Hpp
        glm::glm
)

target_compile_definitions(solaris PRIVATE
    $<$<CONFIG:Release>:NDEBUG>
    GLFW_INCLUDE_VULKAN
)

set(SHADER_DIR        ${CMAKE_SOURCE_DIR}/shaders)
set(SHADER_BUILD_DIR  ${CMAKE_BINARY_DIR}/shaders)
file(MAKE_DIRECTORY ${SHADER_BUILD_DIR})


set(SHADER_EXTS vert frag comp geom tesc tese)

set(SHADER_SOURCES "")
foreach(EXT IN LISTS SHADER_EXTS)
    file(GLOB_RECURSE FOUND_SHADERS CONFIGURE_DEPENDS
        ${SHADER_DIR}/*.${EXT}
    )
    list(APPEND SHADER_SOURCES ${FOUND_SHADERS})
endforeach()

set(SHADER_SPV_BINARIES "")
foreach(SRC ${SHADER_SOURCES})
    get_filename_component(SRC_NAME ${SRC} NAME)        
    set(SPV ${SHADER_BUILD_DIR}/${SRC_NAME}.spv)      
    add_custom_command(
        OUTPUT ${SPV}
        COMMAND ${GLSLC} ${SRC} -o ${SPV}
        DEPENDS ${SRC}
        COMMENT "Compiling shader: ${SRC_NAME}"
        VERBATIM
    )
    list(APPEND SHADER_SPV_BINARIES ${SPV})
endforeach()

add_custom_target(Shaders ALL
    DEPENDS ${SHADER_SPV_BINARIES}
)

add_dependencies(solaris Shaders)

add_custom_command(TARGET solaris POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:solaris>/shaders
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${SHADER_BUILD_DIR} $<TARGET_FILE_DIR:solaris>/shaders
    COMMENT "Copying shaders to runtime directory"
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
