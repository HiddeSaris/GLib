#include "Scene.h"
#include "Window/Window.h"
#include "Renderer/Render.h"
#include "Entity.h"
#include "Systems.h"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

namespace GLib {

    Scene::Scene() {
        m_LastFrameTime = std::chrono::steady_clock::now();
        m_NumEntities = 0;
        Window::ImGuiNewFrame();
    }

    Scene::~Scene() {
        for (auto& system : m_Systems){
            system->OnCleanup(*this);
        }
    }

    Entity Scene::CreateEntity(const std::string& name) {
        m_NumEntities++;
        
        Entity entity(m_Registry.create(), this);
        entity.Add<TransformComponent>();
        auto& tag = entity.Add<TagComponent>(name.empty() ? "Entity" : name);
        return entity;
    }

    void Scene::UpdateSystems()
    {
        CalculateDeltaTime();

        for (auto& system : m_Systems){
            system->OnUpdate(*this, m_DeltaTime);
        }
    }

    void Scene::OnViewportResize(uint32_t width, uint32_t height) {
        
        auto view = m_Registry.view<CameraComponent>();
        for (auto entity : view){
            auto& camera = view.get<CameraComponent>(entity);
            if (!camera.m_FixedAspecRatio){
                camera.m_Camera.SetViewportSize(width, height);
            }
        }
    }

    double Scene::CalculateDeltaTime()
    {
        std::chrono::steady_clock::time_point time = std::chrono::steady_clock::now();
        std::chrono::duration<double> dt = time - m_LastFrameTime;
        m_DeltaTime = dt.count();
        m_LastFrameTime = time;
        return m_DeltaTime;
    }
    
}
