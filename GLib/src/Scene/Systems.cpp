#include "Systems.h"

#include "Asset/AssetManager.h"

#include "glad/glad.h"

namespace GLib {

    // void RenderSystem::OnInit(Scene& scene) {

    // }

    void AssetManagerSystem(Scene &scene) {
        AssetManager::Update();
    }

    void RenderUpdateSystem(Scene &scene)
    {
        Entity camEntity = scene.GetPrimaryCamera();

        if (!camEntity) {
            std::cout << "No primary camera set, add one to render.\n";
            return;
        }
        TransformComponent camTransform = camEntity.Get<TransformComponent>();
        
        auto pointlightgroup = scene.Group<PointLightComponent>();
        for (Entity entity : pointlightgroup) {
            auto light = pointlightgroup.Get<PointLightComponent>(entity);
            auto worldTrans = entity.GetWorldTransform();
            Render::SubmitPointLight(worldTrans.Translation, light.Color, light.Intensity);
        }
        auto dirlightgroup = scene.Group<DirectionalLightComponent>();
        for (Entity entity : dirlightgroup) {
            auto light = dirlightgroup.Get<DirectionalLightComponent>(entity);
            Render::SubmitDirLight(light.Direction, light.Color, light.Intensity);
        }
        auto spotlightgroup = scene.Group<SpotLightComponent>();
        for (Entity entity : spotlightgroup) {
            auto worldTrans = entity.GetWorldTransform();
            auto light = spotlightgroup.Get<SpotLightComponent>(entity);
            Render::SubmitSpotLight(worldTrans.Translation, light.Direction, light.Color, light.Intensity, light.Cutoff, light.OuterCutoff);
        }

        std::vector<std::tuple<std::shared_ptr<Mesh>, std::shared_ptr<Material>, glm::mat4, float>> transparentMeshes;

        auto meshGroup = scene.Group<MeshComponent, MaterialComponent>();
        for (Entity entity : meshGroup) {
            auto& [meshcomp, materialcomp] = meshGroup.Get<MeshComponent, MaterialComponent>(entity);
            if (!AssetManager::IsLoaded(meshcomp.mesh) || !AssetManager::IsLoaded(materialcomp.material)) continue;
            auto mesh = AssetManager::Get(meshcomp.mesh);
            auto material = AssetManager::Get(materialcomp.material);
            auto worldTrans = entity.GetWorldTransform();
            
            if (material->alphaMode == AlphaMode::Blend){
                float d = glm::distance2(camTransform.Translation, worldTrans.Translation);
                transparentMeshes.push_back({mesh, material, worldTrans.GetTransform(), d});
            }
            else {
                Render::Submit(mesh, material, worldTrans.GetTransform());
            }
        }

        scene.Group<VertexArrayComponent>().Each(
            [](Entity entity, auto& vertexArray) {
                auto worldTrans = entity.GetWorldTransform();
                if (AssetManager::IsLoaded(vertexArray.m_Texture)){
                    Render::Submit(vertexArray.m_VertexArray, AssetManager::Get<Texture>(vertexArray.m_Texture), worldTrans.GetTransform());
                }
            });

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

    void GltfUpdateSystem(Scene& scene) {
        for (Entity entity : scene.Group<GltfSceneComponent>()) {
            if (AssetManager::IsLoaded(entity.Get<GltfSceneComponent>().gltf)){
                GltfInitScene(scene, entity);
            }
        }
    }

}