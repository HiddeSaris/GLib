#include "Systems.h"

#include "Asset/AssetManager.h"

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
        
        auto pointlightgroup = scene.GetRegistry().group<PointLightComponent>();
        for (auto entity : pointlightgroup) {
            auto worldTrans = Entity(entity, GetActiveScene()).GetWorldTransform();
            PointLightComponent light = pointlightgroup.get<PointLightComponent>(entity);
            Render::SubmitPointLight(worldTrans.Translation, light.Color, light.Intensity);
        }
        auto dirlightgroup = scene.GetRegistry().group<DirectionalLightComponent>();
        for (auto entity : dirlightgroup) {
            DirectionalLightComponent light = dirlightgroup.get<DirectionalLightComponent>(entity);
            Render::SubmitDirLight(light.Direction, light.Color, light.Intensity);
        }
        auto spotlightgroup = scene.GetRegistry().group<SpotLightComponent>();
        for (auto entity : spotlightgroup) {
            auto worldTrans = Entity(entity, GetActiveScene()).GetWorldTransform();
            SpotLightComponent light = spotlightgroup.get<SpotLightComponent>(entity);
            Render::SubmitSpotLight(worldTrans.Translation, light.Direction, light.Color, light.Intensity, light.Cutoff, light.OuterCutoff);
        }

        std::vector<std::tuple<std::shared_ptr<Mesh>, std::shared_ptr<Material>, glm::mat4, float>> transparentMeshes;

        auto group = scene.GetRegistry().group<MeshComponent, MaterialComponent>();
        for (auto entity : group){
            auto& [meshcomp, materialcomp] = group.get<MeshComponent, MaterialComponent>(entity);
            if (!AssetManager::IsLoaded(meshcomp.mesh) || !AssetManager::IsLoaded(materialcomp.material)) continue;

            auto mesh = AssetManager::Get(meshcomp.mesh);
            auto material = AssetManager::Get(materialcomp.material);
            auto worldTrans = Entity(entity, GetActiveScene()).GetWorldTransform();
            
            if (material->alphaMode == AlphaMode::Blend){
                float d = glm::distance2(camTransform.Translation, worldTrans.Translation);
                transparentMeshes.push_back({mesh, material, worldTrans.GetTransform(), d});
            }
            else {
                Render::Submit(mesh, material, worldTrans.GetTransform());
            }
        }

        auto groupVA = scene.GetRegistry().group<VertexArrayComponent>();
        for (auto entity : groupVA){
            auto& vertexArray = groupVA.get<VertexArrayComponent>(entity);
            auto worldTrans = Entity(entity, GetActiveScene()).GetWorldTransform();
            if (AssetManager::IsLoaded(vertexArray.m_Texture)){
                Render::Submit(vertexArray.m_VertexArray, AssetManager::Get<Texture>(vertexArray.m_Texture), worldTrans.GetTransform());
            }
        }

        std::sort(transparentMeshes.begin(), transparentMeshes.end(),
        [](auto& a, auto& b){ 
            return std::get<3>(a) > std::get<3>(b); 
        });

        glDepthMask(GL_FALSE);

        for (auto& [mesh, material, transform, d] : transparentMeshes){
            Render::Submit(mesh, material, transform);
        }

        glDepthMask(GL_TRUE);


        Render::DrawLineFlush();

    }

    // void RenderSystem::OnCleanup(Scene& scene) {

    // }

}