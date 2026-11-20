#include "Scene.h"
#include "Window/Window.h"
#include "Renderer/Render.h"
#include "Entity.h"
#include "Systems.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"


#include "core/hashed_string.hpp"

namespace GLib {

    Scene::Scene() {
        m_LastFrameTime = std::chrono::steady_clock::now();
        m_NumEntities = 0;
        m_Registry.on_construct<CameraComponent>().connect<&Scene::OnCameraCreated>(this);
    }

    Entity Scene::CreateEntity(entt::entity entity) {
        m_NumEntities++;
        return Entity(entity, this);
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

    Entity Scene::GetPrimaryCamera() const { 
        return m_PrimaryCamera; 
    }

    void Scene::SetPrimaryCamera(Entity cam) {
        if (cam.Has<CameraComponent>()){
            m_PrimaryCamera = cam;
        }
        else {
            std::cout << "Warning [Scene::SetPrimaryCamera]: entity does not have a CameraComponent\n";
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

    void Scene::OnCameraCreated(entt::registry& registry, entt::entity entity) {
        if (!m_PrimaryCamera) {
            m_PrimaryCamera = CreateEntity(entity);
        }
    }

}
