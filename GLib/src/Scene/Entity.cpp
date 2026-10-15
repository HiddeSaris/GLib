#include "Entity.h"

#include "Components.h"
#include "Scene.h"

namespace GLib {

    Entity::Entity(entt::entity handle, std::shared_ptr<Scene> scene)
        : m_EntityHandle(handle), m_Scene(scene) { }

    bool Entity::IsChildOf(Entity ent) {
        Entity cur = *this;
        while(cur) {
            if (!cur.Has<ChildOf>()) {
                return false;
            }
            
            if (cur.Get<ChildOf>().parent.GetHandle() == ent.GetHandle()) {
                return true;
            }
            cur = cur.Get<ChildOf>().parent;
        }
        return false;
    }

    bool Entity::IsParentOf(Entity ent) {
        return ent.IsChildOf(*this);
    }

    void Entity::SetParent(Entity parent)
    {
        if (!parent || !*this) {
            std::cout << "Warning [Entity::SetParent]: Not a valid entity.\n";
        }
        if (parent.GetHandle() == GetHandle()) {
            std::cout << "Warning [Entity::SetParent]: Parent and child are the same.\n";
        }
        if (parent.IsChildOf(*this)) {
            std::cout << "Warning [Entity::SetParent]: Parent is a child of Entity\n";
        }

        auto childOf = GetOrEmplace<ChildOf>();
        Entity oldParent = childOf.parent;
        if (oldParent && oldParent.Has<Children>()) {
            auto& children = oldParent.Get<Children>().children;
            children.erase(std::find(children.begin(), children.end(), *this));
        }

        childOf.parent = parent;
        parent.GetOrEmplace<Children>().children.push_back(*this);
    }

    void Entity::AddChild(Entity child) {
        child.SetParent(*this);
    }

    Entity Entity::CreateChild(const std::string& name) {
        Entity child = m_Scene->CreateEntity(name);
        AddChild(child);
        return child;
    }
};