#include "Bullet.h"

#include "Renderer/Render.h"

namespace GLib {

    glm::vec3 ToGlm(const btVector3& v) { return glm::vec3(v.getX(), v.getY(), v.getZ()); }

    glm::quat ToGlm(const btQuaternion& q) { return glm::quat(q.getW(), q.getX(), q.getY(), q.getZ()); }

    glm::mat4 ToGlm(const btTransform& t) {
        glm::mat4 m;
        const btMatrix3x3& basis = t.getBasis();
        // rotation
        for (int r = 0; r < 3; r++)
        {
            for (int c = 0; c < 3; c++)
            {
                m[c][r] = basis[r][c];
            }
        }
        // translation
        btVector3 origin = t.getOrigin();
        m[3][0] = origin.getX();
        m[3][1] = origin.getY();
        m[3][2] = origin.getZ();
        // unit scale
        m[0][3] = 0.0f;
        m[1][3] = 0.0f;
        m[2][3] = 0.0f;
        m[3][3] = 1.0f;
        return m;
    }

    // glm to bullet type conversion
    btVector3 ToBullet(const glm::vec3& v) { return btVector3(v.x, v.y, v.z); }

    btQuaternion ToBullet(const glm::quat& q) { return btQuaternion(q.x, q.y, q.z, q.w); }

    btMatrix3x3 ToBullet(const glm::mat3& m) { return btMatrix3x3(m[0][0], m[1][0], m[2][0], m[0][1], m[1][1], m[2][1], m[0][2], m[1][2], m[2][2]); }

    btTransform ToBullet(const glm::mat4& m) {
        glm::mat3 m3(m);
        return btTransform(ToBullet(m3), ToBullet(glm::vec3(m[3][0], m[3][1], m[3][2])));
    }

    // motion state
    BulletMotionState::BulletMotionState(entt::registry& registry, entt::entity entity)
        : m_Registry(registry), m_Entity(entity)
    {}

    void BulletMotionState::getWorldTransform(btTransform &t) const {
        t = btTransform(
            ToBullet(m_Registry.get<TransformComponent>(m_Entity).m_Rotation),
            ToBullet(m_Registry.get<TransformComponent>(m_Entity).m_Translation)
        );
    }

    void BulletMotionState::setWorldTransform(const btTransform &t) {
        // translation
        m_Registry.get<TransformComponent>(m_Entity).m_Translation = ToGlm(t.getOrigin());
        // rotation
        m_Registry.get<TransformComponent>(m_Entity).m_Rotation = ToGlm(t.getRotation());
    }

    // Debug drawer
    BulletDebugDrawer::BulletDebugDrawer(int debugMode)
        : m_DebugMode(debugMode)
    {}

    void BulletDebugDrawer::drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color) {
        Render::DrawLine(ToGlm(from), ToGlm(to), ToGlm(color));
    }

    void BulletDebugDrawer::drawContactPoint(const btVector3 &PointOnB, const btVector3 &normalOnB, btScalar distance, int lifeTime, const btVector3 &color) {
        btVector3 to = PointOnB + normalOnB * distance;
        BulletDebugDrawer::drawLine(PointOnB, to, color);
    }

    void BulletDebugDrawer::reportErrorWarning(const char *warningString) {
        std::cout << "Bullet: " << warningString << "\n";
    }

    void BulletDebugDrawer::draw3dText(const btVector3 &location, const char *textString) {
        std::cout << "Bullet text: " << textString << "\n";
    }

}