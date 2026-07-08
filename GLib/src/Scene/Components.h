#pragma once

#include "Renderer/Model.h"
#include "Renderer/Camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "btBulletDynamicsCommon.h"


namespace GLib {

    struct TagComponent
    {
        std::string m_Tag;

        TagComponent() = default;
        TagComponent(const TagComponent&) = default;
        TagComponent(const std::string& tag)
            : m_Tag(tag) {}
    };

    struct TransformComponent
    {
        glm::vec3 m_Translation = { 0.0f, 0.0f, 0.0f };
        glm::vec3 m_Scale = { 1.0f, 1.0f, 1.0f };
        glm::quat m_Rotation = glm::quat(glm::vec3(0.0f, 0.0f, 0.0f));
        
        TransformComponent() = default;
        TransformComponent(const TransformComponent&) = default;
        TransformComponent(const glm::vec3 translation)
            : m_Translation(translation) {}

        glm::vec3 GetEulerAnglesDeg() const {
            return glm::degrees(glm::eulerAngles(m_Rotation));
        }
        float GetPitchDeg() const {
            return glm::degrees(glm::eulerAngles(m_Rotation).x);
        }
        float GetYawDeg() const {
            return glm::degrees(glm::eulerAngles(m_Rotation).y);
        }
        float GetRollDeg() const {
            return glm::degrees(glm::eulerAngles(m_Rotation).z);
        }

        void RotateDeg(glm::vec3 axis, float angle) {
            m_Rotation = glm::angleAxis(glm::radians(angle), glm::normalize(axis)) * m_Rotation;
        }

        void SetEulerAnglesDeg(const glm::vec3& angles) {
            m_Rotation = glm::quat(glm::radians(angles));
        }
        void SetPitchDeg(float pitch) {
            glm::vec3 angles = glm::degrees(glm::eulerAngles(m_Rotation));
            angles.x = pitch;
            m_Rotation = glm::quat(glm::radians(angles));
        }
        void SetYawDeg(float yaw) {
            glm::vec3 angles = glm::degrees(glm::eulerAngles(m_Rotation));
            angles.y = yaw;
            m_Rotation = glm::quat(glm::radians(angles));
        }
        void SetRollDeg(float roll) {
            glm::vec3 angles = glm::degrees(glm::eulerAngles(m_Rotation));
            angles.z = roll;
            m_Rotation = glm::quat(glm::radians(angles));
        }

        glm::vec3 GetRotation() const {
            return m_Rotation * glm::vec3(0.0f, 0.0f, -1.0f);
        }

        glm::vec3 GetUpVector() const {
            return m_Rotation * glm::vec3(0.0f, 1.0f, 0.0f);
        }

        glm::vec3 GetRightVector() const {
            return m_Rotation * glm::vec3(1.0f, 0.0f, 0.0f);
        }

        glm::mat4 GetTransform() const {
            glm::mat4 rotation = glm::toMat4(m_Rotation);

            return glm::translate(glm::mat4(1.0f), m_Translation)
                * rotation
                * glm::scale(glm::mat4(1.0f), m_Scale);
        }
    };

    struct ModelRenderingComponent
    {
        Model m_Model;

        ModelRenderingComponent(const ModelRenderingComponent&) = default;
        ModelRenderingComponent(const Model& model)
            : m_Model(model) {}
        ModelRenderingComponent(const std::string& filepath)
            : m_Model(filepath) {}
    };

    struct CameraComponent 
    {
        Camera m_Camera;
        bool m_Primary = true;
        bool m_FixedAspecRatio = false;

        CameraComponent() = default;
        CameraComponent(const CameraComponent&) = default;
        CameraComponent(const Camera& camera)
            : m_Camera(camera) {}
    };

    struct RigidBodyComponent
    {
        btCollisionShape* m_Shape = nullptr;
        float m_Mass{0.};
        btRigidBody* m_Body = nullptr;

        RigidBodyComponent(const RigidBodyComponent&) = default;
        RigidBodyComponent(btCollisionShape* shape, float mass)
            : m_Shape(shape), m_Mass(mass)
        {}

        void Activate() { m_Body->activate(); }
        void DeActivate() { m_Body->setActivationState(WANTS_DEACTIVATION); }

        void SyncTransform() {
            btTransform transform;
            m_Body->getMotionState()->getWorldTransform(transform);
            m_Body->setWorldTransform(transform);
        }
        
    };

}
