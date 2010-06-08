cmake_minimum_required(VERSION 2.6)
project(ATHENA_INPUTS)

# Settings
if (NOT DEFINED ATHENA_INPUTS_SCRIPTING)
    set(ATHENA_INPUTS_SCRIPTING OFF CACHE BOOL "Enable scripting support (needs Athena-Scripting)")
endif()

if (NOT DEFINED ATHENA_INPUTS_STATIC)
    set(ATHENA_INPUTS_STATIC OFF CACHE BOOL "Build Athena-Inputs as a static library")
endif()

# Set the output paths
if (NOT ATHENA_BINARY_DIR)
    set(ATHENA_BINARY_DIR "${ATHENA_INPUTS_BINARY_DIR}")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${ATHENA_INPUTS_BINARY_DIR}/bin")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${ATHENA_INPUTS_BINARY_DIR}/lib")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${ATHENA_INPUTS_BINARY_DIR}/bin")
endif()

# Import XMake
if (EXISTS "${ATHENA_INPUTS_SOURCE_DIR}/dependencies/xmake/XMake.cmake")
    include("${ATHENA_INPUTS_SOURCE_DIR}/dependencies/xmake/XMake.cmake")
endif()

# Process the dependencies
add_subdirectory(dependencies)

# XMake settings
if (ATHENA_INPUTS_STATIC)
    xmake_init_static(ATHENA_INPUTS)
    xmake_add_link_paths(ATHENA_INPUTS "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}")
    xmake_add_link_targets(ATHENA_INPUTS Athena-Inputs)
else()
    xmake_init_dynamic(ATHENA_INPUTS)
    xmake_add_link_paths(ATHENA_INPUTS "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
    xmake_add_link_targets(ATHENA_INPUTS Athena-Inputs "${XMAKE_ATHENA_CORE_LINK_TARGETS}")
endif()

if (APPLE)
    xmake_add_link_flags(ATHENA_INPUTS "-framework IOKit" "-framework CoreFoundation")
endif()

xmake_add_include_paths(ATHENA_INPUTS ${ATHENA_INPUTS_SOURCE_DIR}/include
                                      ${XMAKE_ATHENA_CORE_INCLUDE_PATHS})

# Subdirectories to process
add_subdirectory(include)
add_subdirectory(src)
