#include "ImGuiDebug.h"

#include "Query.h"
#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"

namespace GLib {

    void ImGuiDebug::ShowImGuiDebugMenu(Scene& scene) {
        ImGui::Begin("Scene Inspector");
        for (Entity ent : scene.View<entt::entity>()) {
            if (!ent.Has<ChildOf>() || !ent.Get<ChildOf>().parent) {
                ShowImGuiEntity(scene, ent);
            }
        }
        ImGui::End();
        ImGui::Begin("Components");
        for (auto&& [id, storage] : scene.GetRegistry().storage()) {
            ShowComponent(scene.GetSelectedEntity(), storage);
        }
        ImGui::End();
    }

    void ImGuiDebug::ShowComponent(Entity entity, entt::sparse_set& storage) {
        if (!storage.contains(entity.GetID())) {
            return;
        }
        void* comp = storage.value(entity.GetID());
        entt::type_info type = storage.type();
        
        if (type == entt::type_id<TagComponent>()) {
            auto* ptr = static_cast<TagComponent*>(comp);
            if (ImGui::CollapsingHeader("TagComponent")) {
                ImGui::InputText("Tag", &ptr->Tag);
            }
        }
        else if (type == entt::type_id<ChildOf>()) {
            auto* ptr = static_cast<ChildOf*>(comp);
            if (ImGui::CollapsingHeader("Childof")) {
                Entity ent = ptr->parent;
                const std::string& tag = ent.Has<TagComponent>() ? ent.Get<TagComponent>().Tag : "unnamed entity";
                if (ImGui::TreeNodeEx(ptr, ImGuiTreeNodeFlags_Bullet, "%s (%d)", tag.c_str(), ent.GetID())) {
                    ImGui::TreePop();
                }
                if (ImGui::BeginDragDropTarget()) {
                    ImGui::EndDragDropTarget();
                }
            }
        }
        else if (type == entt::type_id<Children>()) {
            auto* ptr = static_cast<Children*>(comp);
            if (ImGui::CollapsingHeader("Children")) {
                for (Entity ent : ptr->children) {
                    const std::string& tag = ent.Has<TagComponent>() ? ent.Get<TagComponent>().Tag : "unnamed entity";
                    if (ImGui::TreeNodeEx(tag.c_str(), ImGuiTreeNodeFlags_Bullet, "%s (%d)", tag.c_str(), ent.GetID())) {
                        ImGui::TreePop();
                    }
                }
            }
        }
        else if (type == entt::type_id<TransformComponent>()) {
            auto* ptr = static_cast<TransformComponent*>(comp);
            if (ImGui::CollapsingHeader("Transform")) {
                ImGui::DragFloat3("Translation", glm::value_ptr(ptr->Translation), 0.01f, 0.0f, 0.0f, "%.2f", ImGuiSliderFlags_ColorMarkers);
                glm::vec3 euler = ptr->GetEulerAnglesDeg();
                if (ImGui::DragFloat3("Rotation", glm::value_ptr(euler), 0.05f, 0.0f, 0.0f, "%.2f", ImGuiSliderFlags_ColorMarkers)) {
                    ptr->SetEulerAnglesDeg(euler);
                }
                ImGui::DragFloat3("Scale", glm::value_ptr(ptr->Scale), 0.01f, 0.0f, 0.0f, "%.2f", ImGuiSliderFlags_ColorMarkers);
            }
        }
        else if (type == entt::type_id<MeshComponent>()) {
            auto* ptr = static_cast<MeshComponent*>(comp);
            if (ImGui::CollapsingHeader("Mesh")) {
                if (AssetManager::IsLoaded(ptr->mesh)){
                    ImGui::Text("Mesh handle");
                }
                else {
                    ImGui::Text("Mesh isn't loaded yet!");
                }
            }
        }
        else if (type == entt::type_id<MaterialComponent>()) {
            auto* ptr = static_cast<MaterialComponent*>(comp);
            if (ImGui::CollapsingHeader("Material")) {
                if (AssetManager::IsLoaded(ptr->material)){
                    ImGui::Text("Material handle");
                }
                else {
                    ImGui::Text("Material isn't loaded yet!");
                }
            }
        }
        else if (type == entt::type_id<PointLightComponent>()) {
            auto* ptr = static_cast<PointLightComponent*>(comp);
            if (ImGui::CollapsingHeader("PointLight")) {
                ImGui::ColorEdit3("Color", glm::value_ptr(ptr->Color));
                ImGui::DragFloat("Intensity", &ptr->Intensity, 0.1f, 0.0f, 1000, "%.2f", ImGuiSliderFlags_Logarithmic);
            }
        }
        else if (type == entt::type_id<DirectionalLightComponent>()) {
            auto* ptr = static_cast<DirectionalLightComponent*>(comp);
            if (ImGui::CollapsingHeader("DirectionalLight")) {
                ImGui::DragFloat3("Rotation", glm::value_ptr(ptr->Direction), 0.01f, 0.0f, 0.0f, "%.2f", ImGuiSliderFlags_ColorMarkers);
                ImGui::ColorEdit3("Color", glm::value_ptr(ptr->Color));
                ImGui::DragFloat("Intensity", &ptr->Intensity, 0.1f, 0.0f, 1000, "%.2f", ImGuiSliderFlags_Logarithmic);
            }
        }
        else {
            if (ImGui::CollapsingHeader(std::string{storage.info().name()}.c_str())) {
                ImGui::Text("GLib currently doesnt support viewing all components.");
            }
        }
    }

    void ImGuiDebug::ShowImGuiEntity(Scene& scene, Entity entity) {
        std::string tag = (entity.Has<TagComponent>() && entity.Get<TagComponent>().Tag.size()) ? entity.Get<TagComponent>().Tag : "Unnamed Entity";

        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DrawLinesToNodes;
        if (scene.GetSelectedEntity() == entity) flags |= ImGuiTreeNodeFlags_Selected;

        bool has_children = entity.Has<Children>() && !entity.Get<Children>().children.empty();
        if (!has_children) flags |= ImGuiTreeNodeFlags_Leaf;

        bool open = ImGui::TreeNodeEx(tag.c_str(), flags, "%s (%d)", tag.c_str(), entity.GetID());
        if (ImGui::IsItemClicked()) {
            if (scene.GetSelectedEntity() == entity) {
                scene.GetSelectedEntity() = Entity(entt::null, &scene);
            }
            else {
                scene.GetSelectedEntity() = entity;
            }
        }

        if (open) {
            if (has_children) {
                for (Entity child : entity.Get<Children>().children) {
                    ShowImGuiEntity(scene, child);
                }
            }
            
            ImGui::TreePop();
        }
    }

}