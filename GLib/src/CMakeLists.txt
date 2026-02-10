add_library(GLib
    test.cpp
)

# GLib public headers
target_include_directories(GLib
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include      # GLib.h
        ${CMAKE_CURRENT_SOURCE_DIR}        # test.h
        ${GLFW_ROOT}/include                # glfw3.h
)

# Link GLFW and OpenGL
target_link_libraries(GLib
    PRIVATE glfw3 opengl32
)