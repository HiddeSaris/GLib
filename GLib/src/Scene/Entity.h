#pragma once

#include "entt.hpp"

namespace GLib {

    class Scene;
    struct TransformComponent;

    class Entity {
    public:
        //Entity() = default;
        Entity(entt::entity handle, Scene* scene);
        Entity(const Entity& other) = default;
        
        template<typename T, typename... Args>
        T& Add(Args&&... args) {
            if (Has<T>()){
                std::cout << "Error [AddComponent]: Entity already has component!" << std::endl;
            }
            return m_Scene->GetRegistry().emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
        }

        template<typename T>
        T& Get() {
            if (!Has<T>()){
                std::cout << "Error [GetComponent]: Entity doesnt have component '" << typeid(T).name() << "'!" << std::endl;
            }
            return m_Scene->GetRegistry().get<T>(m_EntityHandle);
        }

        template<typename T>
        bool Has() const {
            return m_Scene->GetRegistry().all_of<T>(m_EntityHandle);
        }

        template<typename T, typename... Args>
        T& GetOrEmplace(Args&&... args) {
            return m_Scene->GetRegistry().get_or_emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
        }

        template<typename T>
        void Remove(){
            if (!Has<T>()){
                std::cout << "Error [RemoveComponent]: Entity doesnt have component!" << std::endl;
            }
            m_Scene->GetRegistry().remove<T>(m_EntityHandle);
        }

        bool IsChildOf(Entity ent);
        bool IsParentOf(Entity ent);
        void SetParent(Entity parent);
        void AddChild(Entity child);

        
        template<typename... Components>
        Entity CreateChild(Components&&... components) {
            Entity child = m_Scene->CreateEntity(std::forward<Components>(components)...);
            AddChild(child);
            return child;
        }

        TransformComponent GetWorldTransform();

        entt::entity GetID() const { return m_EntityHandle; }

        bool operator==(Entity& other);
        operator bool() const { return m_EntityHandle != entt::null && m_Scene != nullptr; }
    private:
        entt::entity m_EntityHandle{ entt::null };
        Scene* m_Scene;
    };

}