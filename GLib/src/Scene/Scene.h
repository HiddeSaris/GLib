#pragma once

#include "entt.hpp"

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
            m_Systems.emplace_back(std::make_unique<T>(std::forward<Args>(args)...));
            std::stable_sort(m_Systems.begin(), m_Systems.end(), 
                [](const auto& a, const auto& b){
                    return a->GetPriority() < b->GetPriority();
                });
        }

        void UpdateSystems();

        void OnViewportResize(uint32_t width, uint32_t height);
        //void Render();
        //void Render(Entity cameraEntity);

        entt::registry& GetRegistry() { return m_Registry; }
        double GetDeltaTime() const { return m_DeltaTime; }

    private:
        entt::registry m_Registry;
        std::vector<std::unique_ptr<System>> m_Systems;
        std::chrono::steady_clock::time_point m_LastFrameTime;
        double m_DeltaTime;
    private:
        double CalculateDeltaTime();
    };

}
