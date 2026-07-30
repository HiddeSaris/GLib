#include "Entity.h"

namespace GLib {

    Entity::Entity(entt::entity handle, std::shared_ptr<Scene> scene)
        : m_EntityHandle(handle), m_Scene(scene)
    {

    }

};