# List the source files
set(SRCS Controller.cpp
)

# Precompiler definitions
add_definitions(-DATHENA_EXPORTS)

# List the include paths
xmake_include_directories(ATHENA_INPUTS)

# Declaration of the library
if (NOT ATHENA_INPUTS_STATIC)
    add_library(Athena-Inputs SHARED ${SRCS})
    xmake_target_link_libraries(Athena-Inputs ATHENA_INPUTS)

    # Set the INSTALL_PATH so that the dynamic library can be local
    set_target_properties(Athena-Inputs PROPERTIES BUILD_WITH_INSTALL_RPATH ON
                                                     INSTALL_NAME_DIR "@rpath"
    )

else()
    add_library(Athena-Inputs STATIC ${SRCS})
endif()
