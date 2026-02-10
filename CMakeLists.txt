cmake_minimum_required(VERSION 3.15)
project(GLib)

# ---- GLFW ----
set(GLFW_ROOT ${PROJECT_SOURCE_DIR}/GLib/vendor/glfw)

add_library(glfw3 STATIC IMPORTED)
set_target_properties(glfw3 PROPERTIES
    IMPORTED_LOCATION ${GLFW_ROOT}/lib-vc2022/glfw3.lib
    INTERFACE_INCLUDE_DIRECTORIES ${GLFW_ROOT}/include
)

add_subdirectory(GLib/src)
add_subdirectory(Example)