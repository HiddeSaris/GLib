#include "Scene.h"
#include "Renderer/Render.h"
#include "Entity.h"
#include "Systems.h"

namespace GLib {

    Scene::Scene() {

    }

    Scene::~Scene() {

    }

    Entity Scene::CreateEntity(const std::string& name) {
        Entity entity(m_Registry.create(), this);
        entity.Add<TransformComponent>();
        auto& tag = entity.Add<TagComponent>();
        tag.m_Tag = name.empty() ? "Entity" : name;
        return entity;
    }

    void Scene::UpdateSystems() {
        for (auto& system : m_Systems){
            system->OnUpdate(*this);
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

    void Scene::Render()
    {
        Entity camEntity;
        {    
            auto view = m_Registry.view<CameraComponent>();
            for (auto entity : view){
                auto& camera = view.get<CameraComponent>(entity);
                if (camera.m_Primary){
                    camEntity = Entity(entity, this);
                    break;
                }
            }
        }

        if (camEntity){
            Render(camEntity);
        }
    }

    void Scene::Render(Entity cameraEntity)
    {
        if (!cameraEntity.Has<CameraComponent>() || !cameraEntity.Has<TransformComponent>()){
            std::cout << "Error [Scene::Render]: Camera entity doesnt have the required components! (Camera & Transform)" << std::endl;
            return;
        }

        auto camera = cameraEntity.Get<CameraComponent>().m_Camera;
        auto camTransform = cameraEntity.Get<TransformComponent>();

        Render::BeginFrame(camera, camTransform.GetTransform(), camTransform.m_Translation);

        auto group = m_Registry.group<ModelRenderingComponent>(entt::get<TransformComponent>);
        for (auto entity : group){
            auto& [transform, model] = group.get<TransformComponent, ModelRenderingComponent>(entity);
            Render::Submit(model.m_Model, transform.GetTransform());
        }

        Render::EndFrame();
    }
    
}
