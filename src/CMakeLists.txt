# List the source files
set(SRCS Controller.cpp
         InputsUnit.cpp
         VirtualController.cpp
)

if (APPLE)
    set(PLATFORM_SRCS osx/MacController.cpp
                      osx/MacControllerManager.cpp
                      osx/MacGamepad.cpp
                      osx/MacKeyboard.cpp
                      osx/MacMouse.cpp
    )
endif()

# Precompiler definitions
add_definitions(-DATHENA_EXPORTS)

# List the include paths
xmake_include_directories(ATHENA_INPUTS)

# Declaration of the library
if (NOT ATHENA_INPUTS_STATIC)
    add_library(Athena-Inputs SHARED ${SRCS} ${PLATFORM_SRCS})
    xmake_target_link_libraries(Athena-Inputs ATHENA_CORE)

    if (APPLE)
        set_target_properties(Athena-Inputs PROPERTIES LINK_FLAGS "-framework IOKit -framework CoreFoundation -framework Carbon")
    endif()
    
    # Set the INSTALL_PATH so that the dynamic library can be local
    set_target_properties(Athena-Inputs PROPERTIES BUILD_WITH_INSTALL_RPATH ON
                                                   INSTALL_NAME_DIR "@rpath"
    )

else()
    add_library(Athena-Inputs STATIC ${SRCS} ${PLATFORM_SRCS})
endif()
