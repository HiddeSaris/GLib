#pragma once

#include "entt.hpp" 

namespace GLib
{
    class Scene;
    class Entity;

    class ImGuiDebug {
    public:
        static void ShowImGuiDebugMenu(Scene& scene);

        static void ShowComponent(Entity entity, entt::sparse_set& storage);
        static void ShowImGuiEntity(Scene& scene, Entity entity);
    };

}
