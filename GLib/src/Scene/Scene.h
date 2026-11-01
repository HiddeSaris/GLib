#pragma once

#include "entt.hpp"

#include "Physics/Bullet.h"
#include "Components.h"
#include "Entity.h"
#include "Query.h"

#include <chrono>

namespace GLib {

    class Entity;
    class System;

    class Scene {
    public:
        Scene();

        Entity CreateEntity(const std::string& name = std::string());
        Entity CreateEntity(entt::entity entity);

        void OnViewportResize(uint32_t width, uint32_t height);

        
        entt::registry& GetRegistry() { return m_Registry; }
        int GetNumEntities() const { return m_NumEntities; }
        glm::vec3& GetGravity() { return m_Gravity; }
        Entity GetPrimaryCamera() const;
        double GetDeltaTime() const { return m_DeltaTime; }
        
        void SetGravity(glm::vec3 gravity) { m_Gravity = gravity; }
        
        void SetPrimaryCamera(Entity cam);

        template<typename... Components, typename... Args>
        auto View(Args&&... args) {
            auto view = m_Registry.view<Components...>(std::forward<Args>(args)...);

            return Query<decltype(view)>(std::move(view), this);
        }

        template<typename... Components, typename... Args>
        auto Group(Args&&... args) {
            auto group = m_Registry.group<Components...>(std::forward<Args>(args)...);

            return Query<decltype(group)>(std::move(group), this);
        }

    private:
        entt::registry m_Registry;
        int m_NumEntities;
        Entity m_PrimaryCamera = Entity(entt::null, this);
        glm::vec3 m_Gravity{0.0f, -9.81f, 0.0f};
        std::chrono::steady_clock::time_point m_LastFrameTime;
        double m_DeltaTime;
    private:
        double CalculateDeltaTime();
        void OnCameraCreated(entt::registry& registry, entt::entity entity);
    };

}
