#pragma once

#include <glm/glm.hpp>

namespace GLib
{
    
    class Camera{
    public:
        Camera() = default;
        virtual ~Camera() = default;

        const glm::mat4& GetView() const { return m_View; }
        const glm::mat4& GetProjection() const { return m_Projection; }
        const glm::mat4& GetViewProjection() const { return m_ViewProjection; }
    protected:
        glm::mat4 m_View = glm::mat4(1.0f);
        glm::mat4 m_Projection = glm::mat4(1.0f);
        glm::mat4 m_ViewProjection = glm::mat4(1.0f);
    };

}
