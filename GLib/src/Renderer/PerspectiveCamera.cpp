#include "PerspectiveCamera.h"

#include <iostream>

namespace GLib {

    PerspectiveCamera::PerspectiveCamera(float fov, float near, float far)
        : m_FOV(glm::radians(fov)), m_Near(near), m_Far(far)
    {
        UpdateProjection();
    }

    void PerspectiveCamera::UpdateView()
    {
        glm::quat q = glm::quat(glm::vec3(glm::radians(m_Pitch), glm::radians(m_Yaw), glm::radians(m_Roll)));
        m_Direction = q * glm::vec3(0.0f, 0.0f, -1.0f);
        m_Up = q * glm::vec3(0.0f, 1.0f, 0.0f);
        m_Right = q * glm::vec3(1.0f, 0.0f, 0.0f);
        m_View = glm::lookAt(m_Position, m_Position + m_Direction, m_Up);

        m_ViewProjection = m_Projection * m_View;
    }
    void PerspectiveCamera::UpdateProjection()
    {
        float aspectRatio = (float)Window::GetWidth() / (float)Window::GetHeight();
        m_Projection = glm::perspective(m_FOV, aspectRatio, m_Near, m_Far);
        
        m_ViewProjection = m_Projection * m_View;
    }
}