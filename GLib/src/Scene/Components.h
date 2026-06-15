#pragma once

#include "Renderer/Model.h"
#include "Renderer/Camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>


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
        float m_Pitch = 0.0f;
        float m_Yaw = 0.0f;
        float m_Roll = 0.0f;

        TransformComponent() = default;
        TransformComponent(const TransformComponent&) = default;
        TransformComponent(const glm::vec3 translation)
            : m_Translation(translation) {}

        glm::quat GetQuaternion() const {
            return glm::quat(glm::vec3(glm::radians(m_Pitch), glm::radians(m_Yaw), glm::radians(m_Roll)));
        }

        glm::vec3 GetRotation() const {
            glm::quat q = GetQuaternion();
            return q * glm::vec3(0.0f, 0.0f, -1.0f);
        }

        glm::vec3 GetUpVector() const {
            glm::quat q = GetQuaternion();
            return q * glm::vec3(0.0f, 1.0f, 0.0f);
        }

        glm::vec3 GetRightVector() const {
            glm::quat q = GetQuaternion();
            return q * glm::vec3(1.0f, 0.0f, 0.0f);
        }

        glm::mat4 GetTransform() const {
            glm::mat4 rotation = glm::toMat4(GetQuaternion());

            return glm::translate(glm::mat4(1.0f), m_Translation)
                * rotation
                * glm::scale(glm::mat4(1.0f), m_Scale);
        }
    };

    struct ModelRenderingComponent
    {
        Model m_Model;

        ModelRenderingComponent() = default;
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

}
