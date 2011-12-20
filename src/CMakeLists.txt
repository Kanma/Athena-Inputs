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


# List the include paths
set(INCLUDE_PATHS "${ATHENA_INPUTS_SOURCE_DIR}/include"
                  "${XMAKE_BINARY_DIR}/include")

include_directories(${INCLUDE_PATHS})

xmake_import_search_paths(ATHENA_CORE)


# Declaration of the library
if (NOT ATHENA_INPUTS_STATIC)
    xmake_create_dynamic_library(ATHENA_INPUTS Athena-Inputs 0.1.0 0.1.0 ${SRCS} ${PLATFORM_SRCS})

    if (APPLE)
        xmake_add_to_property(ATHENA_INPUTS LINK_FLAGS "-framework IOKit -framework CoreFoundation -framework Carbon")
    endif()
else()
    xmake_create_static_library(ATHENA_INPUTS Athena-Inputs ${SRCS} ${PLATFORM_SRCS})
endif()

xmake_project_link(ATHENA_INPUTS ATHENA_CORE)

xmake_add_to_list_property(ATHENA_INPUTS COMPILE_DEFINITIONS "ATHENA_EXPORTS")


# Exportations
xmake_export_include_paths(ATHENA_INPUTS ${INCLUDE_PATHS})

if (APPLE AND ATHENA_INPUTS_STATIC)
    xmake_export_link_flags(ATHENA_INPUTS "-framework IOKit -framework CoreFoundation -framework Carbon")
endif()
