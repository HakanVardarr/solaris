file(GLOB_RECURSE SANDBOX_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)


add_executable(sandbox
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${SANDBOX_SOURCES}
)


target_link_libraries(sandbox PRIVATE solaris)

target_include_directories(sandbox
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include 
)

if(APPLE)
    set_target_properties(sandbox PROPERTIES
        BUILD_RPATH "@loader_path"
        INSTALL_RPATH "@loader_path"
    )
elseif(UNIX)
    set_target_properties(sandbox PROPERTIES
        BUILD_RPATH "\$ORIGIN"
        INSTALL_RPATH "\$ORIGIN"
    )
endif()

if(WIN32)
    add_custom_command(TARGET sandbox POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:solaris> $<TARGET_FILE_DIR:sandbox>/
        COMMENT "Copying solaris DLL next to sandbox.exe"
    )
endif()

find_program(GLSLC glslc HINTS ENV VULKAN_SDK PATH_SUFFIXES bin)
if(NOT GLSLC)
    message(FATAL_ERROR "failed to find glslc.")
endif()

set(SHADER_DIR     ${CMAKE_CURRENT_SOURCE_DIR}/shaders)
set(SHADER_OUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/shaders)
file(MAKE_DIRECTORY ${SHADER_OUT_DIR})

set(SHADER_EXTS vert frag comp geom tesc tese)
set(SHADER_SOURCES "")
foreach(EXT IN LISTS SHADER_EXTS)
    file(GLOB_RECURSE FOUND_SHADERS CONFIGURE_DEPENDS ${SHADER_DIR}/*.${EXT})
    list(APPEND SHADER_SOURCES ${FOUND_SHADERS})
endforeach()

set(SHADER_SPV_BINARIES "")
foreach(SRC ${SHADER_SOURCES})
    get_filename_component(SRC_NAME ${SRC} NAME)
    set(SPV ${SHADER_OUT_DIR}/${SRC_NAME}.spv)

    add_custom_command(
        OUTPUT ${SPV}
        COMMAND ${GLSLC} ${SRC} -o ${SPV}
        DEPENDS ${SRC}
        COMMENT "Compiling shader: ${SRC_NAME}"
        VERBATIM
    )
    list(APPEND SHADER_SPV_BINARIES ${SPV})
endforeach()

add_custom_target(SandboxShaders ALL DEPENDS ${SHADER_SPV_BINARIES})
add_dependencies(sandbox SandboxShaders)

add_custom_command(TARGET sandbox POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:sandbox>/shaders
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${SHADER_OUT_DIR} $<TARGET_FILE_DIR:sandbox>/shaders
    COMMENT "Copying shaders to sandbox runtime directory"
)
