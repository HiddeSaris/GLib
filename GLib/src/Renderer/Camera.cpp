#include "Camera.h"

#include "glm/gtc/matrix_transform.hpp"

namespace GLib {

    Camera::Camera()
    {
        RecalculateProjection();
    }

    void Camera::SetPerspective(float FOV, float nearClip, float farClip)
    {
        m_ProjectionType = ProjectionType::Perspective;
        m_PerspectiveFOV = FOV;
        m_Near = nearClip;
        m_Far = farClip;
        RecalculateProjection();
    }

    void Camera::SetOrthographic(float size, float nearClip, float farClip)
    {
        m_ProjectionType = ProjectionType::Orthographic;
        m_OrthographicSize = size;
        m_Near = nearClip;
        m_Far = farClip;
        RecalculateProjection();
    }

    void Camera::SetViewportSize(uint32_t width, uint32_t height)
    {
        m_AspectRatio = (float)width / (float)height;
        RecalculateProjection();
    }

    void Camera::RecalculateProjection()
    {
        if (m_ProjectionType == ProjectionType::Perspective){
            m_Projection = glm::perspective(glm::radians(m_PerspectiveFOV), m_AspectRatio, m_Near, m_Far);
        }
        else if (m_ProjectionType == ProjectionType::Orthographic){
            float orthoLeft = -m_OrthographicSize * m_AspectRatio * 0.5f;
            float orthoRight = m_OrthographicSize * m_AspectRatio * 0.5f;
            float orthoBottom = -m_OrthographicSize * 0.5f;
            float orthoTop = m_OrthographicSize * 0.5f;

            m_Projection = glm::ortho(orthoLeft, orthoRight, orthoBottom, orthoTop, m_Near, m_Far); 
        }
    }

}