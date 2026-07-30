#include "Systems.h"

#include "glad/glad.h"

namespace GLib {

    void RenderSystem::OnInit(Scene& scene) {

    }

    void RenderSystem::OnUpdate(Scene& scene, double dt) {
        Entity camEntity = scene.GetPrimaryCamera();

        if (camEntity){
            TransformComponent camTransform = camEntity.Get<TransformComponent>();
            
            std::vector<std::pair<entt::entity, float>> transparentModels;

            auto group = scene.GetRegistry().group<ModelComponent>(entt::get<TransformComponent>);
            for (auto entity : group){
                auto& [transform, model] = group.get<TransformComponent, ModelComponent>(entity);
                if (model.m_Model.IsTransparent()){
                    float d = glm::distance2(camTransform.m_Translation, transform.m_Translation);
                    transparentModels.push_back({entity, d});
                }
                else {
                    Render::Submit(model.m_Model, transform.GetTransform());
                }
            }

            auto groupVA = scene.GetRegistry().group<VertexArrayComponent>(entt::get<TransformComponent>);
            for (auto entity : groupVA){
                auto& [transform, vertexArray] = groupVA.get<TransformComponent, VertexArrayComponent>(entity);
                Render::Submit(vertexArray.m_VertexArray, vertexArray.m_Texture, transform.GetTransform());
            }

            std::sort(transparentModels.begin(), transparentModels.end(),
            [](std::pair<entt::entity, float> a, std::pair<entt::entity, float> b){ 
                return a.second > b.second; 
            });

            glDepthMask(GL_FALSE);

            for (auto& [ent, d] : transparentModels){
                auto [transform, model] = group.get<TransformComponent, ModelComponent>(ent);
                Render::Submit(model.m_Model, transform.GetTransform());
            }

            glDepthMask(GL_TRUE);


            Render::DrawLineFlush();
        }

    }

    void RenderSystem::OnCleanup(Scene& scene) {

    }

}