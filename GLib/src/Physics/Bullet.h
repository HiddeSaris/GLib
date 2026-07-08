#pragma once

#include "Scene/Components.h"

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"

#include "entt.hpp"

#include "btBulletDynamicsCommon.h"

namespace GLib {

    // bullet to glm type conversion
    glm::vec3 ToGlm(const btVector3& v);

    glm::quat ToGlm(const btQuaternion& q);

    glm::mat4 ToGlm(const btTransform& t);
    
    // glm to bullet type conversion
    btVector3 ToBullet(const glm::vec3& v);

    btQuaternion ToBullet(const glm::quat& q);

    btMatrix3x3 ToBullet(const glm::mat3& m);

    btTransform ToBullet(const glm::mat4& m);

    
    class BulletMotionState : public btMotionState {
    public:
        BulletMotionState(entt::registry& registry, entt::entity entity);
        void getWorldTransform(btTransform& t) const override;
        void setWorldTransform(const btTransform& t) override;
    private:
        entt::registry& m_Registry;
        entt::entity m_Entity;
    };

    class BulletDebugDrawer : public btIDebugDraw {
    public:
        BulletDebugDrawer() = default;
        BulletDebugDrawer(int debugMode);
            
        virtual void drawLine(const btVector3& from,const btVector3& to,const btVector3& color);
        virtual void drawContactPoint(const btVector3& PointOnB,const btVector3& normalOnB,btScalar distance,int lifeTime,const btVector3& color);
        virtual void reportErrorWarning(const char* warningString);
        virtual void draw3dText(const btVector3& location,const char* textString);
        virtual void setDebugMode(int debugMode) { m_DebugMode = debugMode; }
        virtual int getDebugMode() const { return m_DebugMode;}
    private:
        int m_DebugMode = 1;
    };

}