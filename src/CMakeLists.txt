cmake_minimum_required (VERSION 2.6)
project (weston-spice)

# The version number.
set (WESRON_SPICE_VERSION_MAJOR 0)
set (WESTON_SPICE_VERSION_MINOR 0)
set (DEBUG 2)

set (PROJECTS_DIR "/home/shved/Projects")
set (SPICE_DIR "${PROJECTS_DIR}/Spice")
set (WAYLAND_DIR "${PROJECTS_DIR}/Wayland")

set (CURRENT_DIR "${WAYLAND_DIR}/weston-spice")
set (PREFIX "${SPICE_DIR}/install")
set (PROJECT_BINARY_DIR "${CURRENT_DIR}/bin")

set (LIBRARY_OUTPUT_PATH "${CURRENT_DIR}/obj")
set (EXECUTABLE_OUTPUT_PATH "${CURRENT_DIR}/bin")

set (CMAKE_C_FLAGS "-g")

set (CMAKE_INCLUDE_DIRECTORIES_BEFORE on)

configure_file (
  "${PROJECT_SOURCE_DIR}/compositor-spice-conf.h.in"
  "${PROJECT_SOURCE_DIR}/compositor-spice-conf.h"
  )

include_directories (
    "${PROJECT_BINARY_DIR}" 
    "${PREFIX}/include"  
    "${PREFIX}/include/spice-server"
    "${PREFIX}/include/spice-1"
    "/usr/include/pixman-1/"
    )
link_directories (
    "${PROJECT_BINARY_DIR}"
    "${PREFIX}/lib"
    )

add_library ( compositor-spice MODULE
              "${PROJECT_SOURCE_DIR}/compositor-spice.c"
              "${PROJECT_SOURCE_DIR}/shared/config-parser.c"
              "${PROJECT_SOURCE_DIR}/shared/option-parser.c"
              "${PROJECT_SOURCE_DIR}/weston_basic_event_loop.c"
              "${PROJECT_SOURCE_DIR}/weston_qxl_interface.c"
              "${PROJECT_SOURCE_DIR}/weston_qxl_commands.c"
              "${PROJECT_SOURCE_DIR}/weston_input_interfaces.c"
            )
add_custom_command ( TARGET compositor-spice
                     POST_BUILD
                     COMMAND cp ARGS "${LIBRARY_OUTPUT_PATH}/libcompositor-spice.so" "${PREFIX}/lib/weston/spice-backend.so"
                   )
target_link_libraries   ( compositor-spice 
                          "wayland-server"
                          "spice-server"
                        )
