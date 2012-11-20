# List the headers files
set(HEADERS ${XMAKE_BINARY_DIR}/include/Athena-Inputs/Config.h
            ../include/Athena-Inputs/Controller.h
            ../include/Athena-Inputs/Declarations.h
            ../include/Athena-Inputs/Gamepad.h
            ../include/Athena-Inputs/IEventsListener.h
            ../include/Athena-Inputs/InputsUnit.h
            ../include/Athena-Inputs/IVirtualEventsListener.h
            ../include/Athena-Inputs/Keyboard.h
            ../include/Athena-Inputs/Mouse.h
            ../include/Athena-Inputs/Prerequisites.h
            ../include/Athena-Inputs/VirtualController.h
)


# List the source files
set(SRCS Controller.cpp
         Gamepad.cpp
         InputsUnit.cpp
         Keyboard.cpp
         Mouse.cpp
         VirtualController.cpp
)


# List the include paths
set(INCLUDE_PATHS "${ATHENA_INPUTS_SOURCE_DIR}/include"
                  "${XMAKE_BINARY_DIR}/include")

include_directories(${INCLUDE_PATHS})

xmake_import_search_paths(ATHENA_CORE)
xmake_import_search_paths(OIS)


# Declaration of the library
xmake_create_dynamic_library(ATHENA_INPUTS Athena-Inputs
                             "${ATHENA_INPUTS_VERSION_MAJOR}.${ATHENA_INPUTS_VERSION_MINOR}.${ATHENA_INPUTS_VERSION_PATCH}"
                             "${ATHENA_INPUTS_VERSION_MAJOR}.${ATHENA_INPUTS_VERSION_MINOR}.${ATHENA_INPUTS_VERSION_PATCH}"
                             ${HEADERS} ${SRCS})

xmake_add_to_list_property(ATHENA_INPUTS COMPILE_DEFINITIONS "ATHENA_INPUTS_EXPORTS")

if (APPLE)
    xmake_add_to_property(ATHENA_INPUTS LINK_FLAGS "-framework IOKit -framework CoreFoundation -framework Carbon -framework Cocoa")
endif()

xmake_project_link(ATHENA_INPUTS ATHENA_CORE)
xmake_project_link(ATHENA_INPUTS OIS)


# Exportations
xmake_export_include_paths(ATHENA_INPUTS ${INCLUDE_PATHS})
