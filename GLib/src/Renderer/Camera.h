#pragma once

#include <glm/glm.hpp>

namespace GLib
{

    enum class ProjectionType {
        Perspective,
        Orthographic,
    };
    
    class Camera{
    public:
        Camera();
        ~Camera() = default;

        void SetPerspective(float FOV, float nearClip, float farClip);
        void SetOrthographic(float size, float nearClip, float farClip);

        void SetViewportSize(uint32_t width, uint32_t height);

        float GetPerspectiveFOV() const { return m_PerspectiveFOV; }
        void SetPerspectiveFOV(float FOV) { m_PerspectiveFOV = FOV; RecalculateProjection(); }

        float GetOrthographicSize() const { return m_OrthographicSize; }
        void SetOrthographicSize(float size) { m_OrthographicSize = size; RecalculateProjection(); }

        float GetNearClip() const { return m_Near; }
        void SetNearClip(float near) { m_Near = near; RecalculateProjection(); }
        float GetFarClip() const { return m_Far; }
        void SetFarClip(float far) { m_Far = far; RecalculateProjection(); }
        
        ProjectionType GetProjectionType() const { return m_ProjectionType; }
        void SetProjectionType(ProjectionType type) { m_ProjectionType = type; RecalculateProjection(); }
        
        const glm::mat4& GetProjection() const { return m_Projection; }
    private:
        void RecalculateProjection();
    private:
        ProjectionType m_ProjectionType = ProjectionType::Perspective;

        glm::mat4 m_Projection = glm::mat4(1.0f);

        float m_PerspectiveFOV = glm::radians(45.0f);

        float m_OrthographicSize = 10.0f;
    
        float m_Near = 0.01f, m_Far = 1000.0f;
        float m_AspectRatio = 1.0f;
    };

}
