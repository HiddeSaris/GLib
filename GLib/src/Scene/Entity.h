#pragma once

#include "entt.hpp"

#include "Scene.h"

namespace GLib {

    class Entity {
    public:
        Entity() = default;
        Entity(entt::entity handle, std::shared_ptr<Scene> scene);
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
        bool Has(){
            return m_Scene->GetRegistry().all_of<T>(m_EntityHandle);
        }

        template<typename T>
        void Remove(){
            if (!Has<T>()){
                std::cout << "Error [RemoveComponent]: Entity doesnt have component!" << std::endl;
            }
            m_Scene->m_Registry.remove<T>(m_EntityHandle);
        }

        entt::entity GetHandle() const { return m_EntityHandle; }

        operator bool() const { return m_EntityHandle != entt::null; }
    private:
        entt::entity m_EntityHandle{ entt::null };
        std::shared_ptr<Scene> m_Scene;
    };

}