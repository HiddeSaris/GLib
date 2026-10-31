#pragma once

#include "Scene/Scene.h"
#include "Window/Window.h"

#include <unordered_map>

namespace GLib {

    enum class Stage {
        Setup,
        PreUpdate,
        Update,
        PostUpdate,
    };

    class App {
    public:
        App();

        using System = std::function<void(Scene&)>;

        void Update();
        void Run();

        void Exit() { m_ShouldExit = true; }
        bool ShouldExit() const { return m_ShouldExit || Window::ShouldClose(); }

        Scene& GetScene() { return m_Scene; }

        template<typename... Systems>
        App& AddSystems(Stage stage, Systems&&... systems) {
            (m_Systems[stage].emplace_back(std::forward<Systems>(systems)), ...);
            return *this;
        }

    private:
        Scene m_Scene;
        std::unordered_map<Stage, std::vector<System>> m_Systems;

        bool m_ShouldExit = false;
    };

}