# ---- GLM ----
set(GLM_ROOT ${PROJECT_SOURCE_DIR}/GLib/vendor/glm)

add_library(glm INTERFACE)
target_include_directories(glm INTERFACE ${GLM_ROOT})

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

# ---- STB_IMAGE ----
set(STB_ROOT ${PROJECT_SOURCE_DIR}/GLib/vendor/stb_image)

add_library(stb_image STATIC ${STB_ROOT}/include/stb_image.cpp)
target_include_directories(stb_image PUBLIC ${STB_ROOT}/include)

# ---- ASSIMP ----
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL" CACHE STRING "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(CMAKE_DEBUG_POSTFIX "" CACHE STRING "" FORCE)

set(CMAKE_BUILD_TYPE_SAVED ${CMAKE_BUILD_TYPE})
set(CMAKE_BUILD_TYPE Release)
add_subdirectory(${PROJECT_SOURCE_DIR}/GLib/vendor/assimp ${CMAKE_BINARY_DIR}/assimp)
set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE_SAVED})

#set_property(TARGET assimp PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

# ---- GLib library ----

add_library(GLib STATIC
    Window.cpp
    Input.cpp
    Renderer/Buffer.cpp
    Renderer/BufferLayout.cpp
    Renderer/PerspectiveCamera.cpp
    Renderer/Render.cpp
    Renderer/Shader.cpp
    Renderer/VertexArray.cpp
    Renderer/Texture.cpp
    Renderer/Model.cpp
    Renderer/Mesh.cpp
)

target_include_directories(GLib
    PUBLIC
        ${PROJECT_SOURCE_DIR}/GLib/include
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${GLM_ROOT}
)

target_link_libraries(GLib
    PUBLIC glm assimp
    PRIVATE glad glfw3 opengl32 stb_image
)