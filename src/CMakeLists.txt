# List the headers files
set(HEADERS ${XMAKE_BINARY_DIR}/include/Athena-Inputs/Config.h
            ../include/Athena-Inputs/Controller.h
            ../include/Athena-Inputs/Declarations.h
            ../include/Athena-Inputs/IControllerManager.h
            ../include/Athena-Inputs/IEventsListener.h
            ../include/Athena-Inputs/InputsUnit.h
            ../include/Athena-Inputs/IVirtualEventsListener.h
            ../include/Athena-Inputs/Prerequisites.h
            ../include/Athena-Inputs/VirtualController.h
)

if (APPLE)
    set(PLATFORM_HEADERS ../include/Athena-Inputs/osx/MacController.h
                         ../include/Athena-Inputs/osx/MacControllerManager.h
                         ../include/Athena-Inputs/osx/MacGamepad.h
                         ../include/Athena-Inputs/osx/MacHelpers.h
                         ../include/Athena-Inputs/osx/MacKeyboard.h
                         ../include/Athena-Inputs/osx/MacMouse.h
    )
endif()


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
xmake_create_dynamic_library(ATHENA_INPUTS Athena-Inputs
                             "${ATHENA_INPUTS_VERSION_MAJOR}.${ATHENA_INPUTS_VERSION_MINOR}.${ATHENA_INPUTS_VERSION_PATCH}"
                             "${ATHENA_INPUTS_VERSION_MAJOR}.${ATHENA_INPUTS_VERSION_MINOR}.${ATHENA_INPUTS_VERSION_PATCH}"
                             ${HEADERS} ${PLATFORM_HEADERS} ${SRCS} ${PLATFORM_SRCS})

xmake_add_to_list_property(ATHENA_INPUTS COMPILE_DEFINITIONS "ATHENA_INPUTS_EXPORTS")

if (APPLE)
    xmake_add_to_property(ATHENA_INPUTS LINK_FLAGS "-framework IOKit -framework CoreFoundation -framework Carbon")
endif()

xmake_project_link(ATHENA_INPUTS ATHENA_CORE)


# Exportations
xmake_export_include_paths(ATHENA_INPUTS ${INCLUDE_PATHS})
