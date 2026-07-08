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

# ---- IMGUI ----
set(IMGUI_ROOT ${PROJECT_SOURCE_DIR}/GLib/vendor/imgui)

add_library(imgui STATIC 
    ${IMGUI_ROOT}/imgui.cpp
    ${IMGUI_ROOT}/imgui_demo.cpp
    ${IMGUI_ROOT}/imgui_draw.cpp
    ${IMGUI_ROOT}/imgui_tables.cpp
    ${IMGUI_ROOT}/imgui_widgets.cpp
    ${IMGUI_ROOT}/backends/imgui_impl_glfw.cpp
    ${IMGUI_ROOT}/backends/imgui_impl_opengl3.cpp
    ${IMGUI_ROOT}/misc/cpp/imgui_stdlib.cpp
)

target_include_directories(imgui PUBLIC ${IMGUI_ROOT} ${IMGUI_ROOT}/backends)

target_link_libraries(imgui
    PUBLIC glfw3
    PRIVATE glad opengl32
)

# ---- ENTT ----
set(ENTT_ROOT ${PROJECT_SOURCE_DIR}/GLib/vendor/entt)

add_library(entt INTERFACE)
target_include_directories(entt INTERFACE ${ENTT_ROOT}/include/entt)

# ---- BULLET ----
set(BULLET_ROOT ${PROJECT_SOURCE_DIR}/GLib/vendor/bullet)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

set(BUILD_BULLET2_DEMOS OFF CACHE BOOL "" FORCE)
set(BUILD_BULLET3         OFF CACHE BOOL "" FORCE)
set(BUILD_CLSOCKET        OFF CACHE BOOL "" FORCE)
set(BUILD_CPU_DEMOS       OFF CACHE BOOL "" FORCE)
set(BUILD_ENET            OFF CACHE BOOL "" FORCE)
set(BUILD_EXTRAS          OFF CACHE BOOL "" FORCE)
set(BUILD_OPENGL3_DEMOS   OFF CACHE BOOL "" FORCE)
set(BUILD_PYBULLET        OFF CACHE BOOL "" FORCE)
set(BUILD_UNIT_TESTS      OFF CACHE BOOL "" FORCE)
set(USE_GRAPHICAL_BENCHMARK OFF CACHE BOOL "" FORCE)
set(USE_GLUT              OFF CACHE BOOL "" FORCE)

add_subdirectory(${BULLET_ROOT} ${CMAKE_BINARY_DIR}/bullet)

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
    Window/Window.cpp
    Window/Input.cpp
    Window/ImGuiBuild.cpp
    Renderer/Buffer.cpp
    Renderer/BufferLayout.cpp
    Renderer/Camera.cpp
    Renderer/PerspectiveCamera.cpp
    Renderer/Render.cpp
    Renderer/Shader.cpp
    Renderer/VertexArray.cpp
    Renderer/Texture.cpp
    Renderer/Model.cpp
    Renderer/Mesh.cpp
    Scene/Scene.cpp
    Scene/Entity.cpp
    Scene/Systems.cpp
    Physics/Bullet.cpp
)

target_include_directories(GLib
    PUBLIC
        ${PROJECT_SOURCE_DIR}/GLib/include
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${GLM_ROOT}
        ${BULLET_ROOT}/src
)

target_link_libraries(GLib
    PUBLIC  glm assimp imgui entt LinearMath BulletCollision BulletDynamics
    PRIVATE glad opengl32 glfw3 stb_image
)