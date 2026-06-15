#pragma once

#include "Scene.h"
#include "Entity.h"
#include "Renderer/Render.h"

namespace GLib {

    class Scene;
    class Entity;

    struct System {
        System() = default;
        ~System() = default;

        virtual void OnUpdate(Scene& scene) {}

        virtual int GetPriority() const { return 0; } // lower = earlier
    };

    struct RenderSystem : public System {
        void OnUpdate(Scene& scene) override {
            Entity camEntity;
            {    
                auto view = scene.GetRegistry().view<CameraComponent>();
                for (auto entity : view){
                    auto& camera = view.get<CameraComponent>(entity);
                    if (camera.m_Primary){
                        camEntity = Entity(entity, &scene);
                        break;
                    }
                }
            }

            if (camEntity){
                Camera camera = camEntity.Get<CameraComponent>().m_Camera;
                TransformComponent camTransform = camEntity.Get<TransformComponent>();

                Render::BeginFrame(camera, camTransform.GetTransform(), camTransform.m_Translation);

                auto group = scene.GetRegistry().group<ModelRenderingComponent>(entt::get<TransformComponent>);
                for (auto entity : group){
                    auto& [transform, model] = group.get<TransformComponent, ModelRenderingComponent>(entity);
                    Render::Submit(model.m_Model, transform.GetTransform());
                }

                Render::EndFrame();
            }

        }

        int GetPriority() const override {
            return INT_MAX - 100;
        }
    };

}