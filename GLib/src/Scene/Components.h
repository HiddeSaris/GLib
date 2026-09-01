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
        std::string Tag;

        TagComponent() = default;
        TagComponent(const TagComponent&) = default;
        TagComponent(const std::string& tag)
            : Tag(tag) {}
    };

    struct TransformComponent
    {
        glm::vec3 Translation = { 0.0f, 0.0f, 0.0f };
        glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };
        glm::quat Rotation = glm::quat(glm::vec3(0.0f, 0.0f, 0.0f));
        
        TransformComponent() = default;
        TransformComponent(const TransformComponent&) = default;
        TransformComponent(const glm::vec3 translation)
            : Translation(translation) {}

        glm::vec3 GetEulerAnglesDeg() const {
            return glm::degrees(glm::eulerAngles(Rotation));
        }
        float GetPitchDeg() const {
            return glm::degrees(glm::eulerAngles(Rotation).x);
        }
        float GetYawDeg() const {
            return glm::degrees(glm::eulerAngles(Rotation).y);
        }
        float GetRollDeg() const {
            return glm::degrees(glm::eulerAngles(Rotation).z);
        }

        void RotateDeg(glm::vec3 axis, float angle) {
            Rotation = glm::angleAxis(glm::radians(angle), glm::normalize(axis)) * Rotation;
        }

        void SetEulerAnglesDeg(const glm::vec3& angles) {
            Rotation = glm::quat(glm::radians(angles));
        }
        void SetPitchDeg(float pitch) {
            glm::vec3 angles = glm::degrees(glm::eulerAngles(Rotation));
            angles.x = pitch;
            Rotation = glm::quat(glm::radians(angles));
        }
        void SetYawDeg(float yaw) {
            glm::vec3 angles = glm::degrees(glm::eulerAngles(Rotation));
            angles.y = yaw;
            Rotation = glm::quat(glm::radians(angles));
        }
        void SetRollDeg(float roll) {
            glm::vec3 angles = glm::degrees(glm::eulerAngles(Rotation));
            angles.z = roll;
            Rotation = glm::quat(glm::radians(angles));
        }

        glm::vec3 GetRotation() const {
            return Rotation * glm::vec3(0.0f, 0.0f, -1.0f);
        }

        glm::vec3 GetUpVector() const {
            return Rotation * glm::vec3(0.0f, 1.0f, 0.0f);
        }

        glm::vec3 GetRightVector() const {
            return Rotation * glm::vec3(1.0f, 0.0f, 0.0f);
        }

        glm::mat4 GetTransform() const {
            glm::mat4 rotation = glm::toMat4(Rotation);

            return glm::translate(glm::mat4(1.0f), Translation)
                * rotation
                * glm::scale(glm::mat4(1.0f), Scale);
        }
    };

    struct ModelComponent
    {
        Model m_Model;

        ModelComponent(const ModelComponent&) = default;
        ModelComponent(const Model& model)
            : m_Model(model) {}
        ModelComponent(const std::string& filepath)
            : m_Model(filepath) {}
    };

    struct PointLightComponent {
        glm::vec3 Color = glm::vec3(1.0f);
        float Intensity = 1.0f;

        PointLightComponent() = default;
        PointLightComponent(const PointLightComponent&) = default;
        PointLightComponent(const glm::vec3& color)
            : Color(color) {}
        PointLightComponent(const glm::vec3& color, float intensity)
            : Color(color), Intensity(intensity) {}
        PointLightComponent(float intensity)
            : Intensity(intensity) {}
    };

    struct DirectionalLightComponent {
        glm::vec3 Direction = glm::vec3(0.0f, 0.0f, 1.0f);
        glm::vec3 Color = glm::vec3(1.0f);
        float Intensity = 1.0f;

        DirectionalLightComponent() = default;
        DirectionalLightComponent(const DirectionalLightComponent&) = default;
        DirectionalLightComponent(const glm::vec3& direction, const glm::vec3& color)
            : Direction(direction), Color(color) {}
        DirectionalLightComponent(const glm::vec3& direction, const glm::vec3& color, float intensity)
            : Direction(direction), Color(color), Intensity(intensity) {}
        DirectionalLightComponent(const glm::vec3& direction, float intensity)
            : Direction(direction), Intensity(intensity) {}
    };

    struct SpotLightComponent {
        glm::vec3 Direction = glm::vec3(0.0f, 0.0f, 1.0f);
        float Cutoff = 40.0f;
        float OuterCutoff = 45.0f;
        glm::vec3 Color = glm::vec3(1.0f);
        float Intensity = 1.0f;

        SpotLightComponent() = default;
        SpotLightComponent(const SpotLightComponent&) = default;
        SpotLightComponent(const glm::vec3& direction, const glm::vec3& color, float cutoff, float outerCutoff)
            : Direction(direction), Color(color), Cutoff(cutoff), OuterCutoff(outerCutoff) {}
        SpotLightComponent(const glm::vec3& direction, const glm::vec3& color, float intensity, float cutoff, float outerCutoff)
            : Direction(direction), Color(color), Intensity(intensity), Cutoff(cutoff), OuterCutoff(outerCutoff) {}
        SpotLightComponent(const glm::vec3& direction, const glm::vec3& color, float intensity)
            : Direction(direction), Color(color), Intensity(intensity) {}
        SpotLightComponent(const glm::vec3& direction, float intensity)
            : Direction(direction), Intensity(intensity) {}
    };

    struct VertexArrayComponent
    {
        std::shared_ptr<VertexArray> m_VertexArray;
        std::shared_ptr<Texture> m_Texture;

        VertexArrayComponent(const VertexArrayComponent&) = default;
        VertexArrayComponent(std::shared_ptr<VertexArray> vertexArray)
            : m_VertexArray(std::move(vertexArray)), m_Texture(std::make_shared<Texture>(TextureSpecification())) {}
        VertexArrayComponent(std::shared_ptr<VertexArray> vertexArray, std::shared_ptr<Texture> texture)
            : m_VertexArray(std::move(vertexArray)), m_Texture(std::move(texture)) {}
    };

    struct CameraComponent 
    {
        Camera m_Camera;
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
