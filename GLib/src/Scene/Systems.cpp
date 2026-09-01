#include "Systems.h"

#include "glad/glad.h"

namespace GLib {

    // void RenderSystem::OnInit(Scene& scene) {

    // }

    void RenderSystem::OnUpdate(Scene& scene, double dt) {
        Entity camEntity = scene.GetPrimaryCamera();

        if (!camEntity) {
            std::cout << "No primary camera set, add one to render.\n";
            return;
        }
        TransformComponent camTransform = camEntity.Get<TransformComponent>();
        
        auto pointlightgroup = scene.GetRegistry().group<PointLightComponent>(entt::get<TransformComponent>);
        for (auto entity : pointlightgroup) {
            glm::vec3 pos = pointlightgroup.get<TransformComponent>(entity).Translation;
            PointLightComponent light = pointlightgroup.get<PointLightComponent>(entity);
            Render::SubmitPointLight(pos, light.Color, light.Intensity);
        }
        auto dirlightgroup = scene.GetRegistry().group<DirectionalLightComponent>();
        for (auto entity : dirlightgroup) {
            DirectionalLightComponent light = dirlightgroup.get<DirectionalLightComponent>(entity);
            Render::SubmitDirLight(light.Direction, light.Color, light.Intensity);
        }
        auto spotlightgroup = scene.GetRegistry().group<SpotLightComponent>(entt::get<TransformComponent>);
        for (auto entity : spotlightgroup) {
            glm::vec3 pos = spotlightgroup.get<TransformComponent>(entity).Translation;
            SpotLightComponent light = spotlightgroup.get<SpotLightComponent>(entity);
            Render::SubmitSpotLight(pos, light.Direction, light.Color, light.Intensity, light.Cutoff, light.OuterCutoff);
        }

        std::vector<std::tuple<std::shared_ptr<Mesh>, glm::mat4, float>> transparentMeshes;

        auto group = scene.GetRegistry().group<ModelComponent>(entt::get<TransformComponent>);
        for (auto entity : group){
            auto& [transform, model] = group.get<TransformComponent, ModelComponent>(entity);

            for (std::shared_ptr<Mesh> mesh : model.m_Model.GetMeshes()){
                if (mesh->IsTransparent()){
                    float d = glm::distance2(camTransform.Translation, transform.Translation);
                    transparentMeshes.push_back({mesh, transform.GetTransform(), d});
                }
                else {
                    Render::Submit(mesh, transform.GetTransform());
                }
            }
        }

        auto groupVA = scene.GetRegistry().group<VertexArrayComponent>(entt::get<TransformComponent>);
        for (auto entity : groupVA){
            auto& [transform, vertexArray] = groupVA.get<TransformComponent, VertexArrayComponent>(entity);
            Render::Submit(vertexArray.m_VertexArray, vertexArray.m_Texture, transform.GetTransform());
        }

        std::sort(transparentMeshes.begin(), transparentMeshes.end(),
        [](auto& a, auto& b){ 
            return std::get<2>(a) > std::get<2>(b); 
        });

        glDepthMask(GL_FALSE);

        for (auto& [mesh, transform, d] : transparentMeshes){
            Render::Submit(mesh, transform);
        }

        glDepthMask(GL_TRUE);


        Render::DrawLineFlush();

    }

    // void RenderSystem::OnCleanup(Scene& scene) {

    // }

}