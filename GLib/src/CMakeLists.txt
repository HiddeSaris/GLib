# ---- GLAD ----
set(GLAD_ROOT ${PROJECT_SOURCE_DIR}/GLib/vendor/glad)

add_library(glad STATIC
    ${GLAD_ROOT}/src/glad.c
)

target_include_directories(glad
    PUBLIC ${GLAD_ROOT}/include
)

# ---- GLFW  ----

set(GLFW_ROOT ${PROJECT_SOURCE_DIR}/GLib/vendor/glfw)

add_library(glfw3 STATIC IMPORTED)

set_target_properties(glfw3 PROPERTIES
    IMPORTED_LOCATION
        ${GLFW_ROOT}/lib-vc2022/glfw3.lib
    INTERFACE_INCLUDE_DIRECTORIES
        ${GLFW_ROOT}/include
)

# ---- GLib library ----

add_library(GLib
    Window.cpp
    Input.cpp
)

target_include_directories(GLib
    PUBLIC
        ${PROJECT_SOURCE_DIR}/GLib/include
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(GLib
    PRIVATE glad glfw3 opengl32
)