#pragma once

#include "entt.hpp"

#include "Physics/Bullet.h"
#include "Components.h"

#include <chrono>

namespace GLib {

    class Entity;
    class System;

    class Scene {
    public:
        Scene();
        ~Scene();

        Entity CreateEntity(const std::string& name = std::string());

        template<typename T, typename... Args>
        void AddSystem(Args&&... args){
            static_assert(std::is_base_of<System, T>::value);
            auto sys = std::make_unique<T>(std::forward<Args>(args)...);
            sys->OnInit(*this);
            m_Systems.push_back(std::move(sys));
            std::stable_sort(m_Systems.begin(), m_Systems.end(), 
                [](const auto& a, const auto& b){
                    return a->GetPriority() < b->GetPriority();
                });
        }

        void UpdateSystems();

        void OnViewportResize(uint32_t width, uint32_t height);

        entt::registry& GetRegistry() { return m_Registry; }
        int GetNumEntities() const { return m_NumEntities; }
        glm::vec3& GetGravity() { return m_Gravity; }
        double GetDeltaTime() const { return m_DeltaTime; }

        void SetGravity(glm::vec3 gravity) { m_Gravity = gravity; }

    private:
        entt::registry m_Registry;
        std::vector<std::unique_ptr<System>> m_Systems;
        int m_NumEntities;
        glm::vec3 m_Gravity{0.0f, -9.81f, 0.0f};
        std::chrono::steady_clock::time_point m_LastFrameTime;
        double m_DeltaTime;
    private:
        double CalculateDeltaTime();
    };

}
