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
            
            if (cur.Get<ChildOf>().parent.GetID() == ent.GetID()) {
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
        if (parent.GetID() == GetID()) {
            std::cout << "Warning [Entity::SetParent]: Parent and child are the same.\n";
        }
        if (parent.IsChildOf(*this)) {
            std::cout << "Warning [Entity::SetParent]: Parent is a child of Entity\n";
        }

        auto& childOf = GetOrEmplace<ChildOf>();
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
    
    TransformComponent Entity::GetWorldTransform() {
        if (!Has<TransformComponent>()) {
            std::cout << "Error [Entity::GetWorldTransform]: Entity doesnt have a TransformComponent!\n";
            return TransformComponent();
        }

        Entity cur = *this;
        TransformComponent trans = Get<TransformComponent>();
        while(cur.Has<ChildOf>() && cur.Get<ChildOf>().parent) {
            cur = cur.Get<ChildOf>().parent;
            if (!cur.Has<TransformComponent>()) break;
            const TransformComponent& curTrans = cur.Get<TransformComponent>();
            trans.Translation = curTrans.Translation + curTrans.Rotation * (curTrans.Scale * trans.Translation);
            trans.Rotation = curTrans.Rotation * trans.Rotation;
            trans.Scale = curTrans.Scale * trans.Scale;
        }

        return trans;
    }
};