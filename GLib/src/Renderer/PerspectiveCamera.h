#pragma once

#include "Camera.h"
#include "Window.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

namespace GLib
{
    
    // class PerspectiveCamera : public Camera{
    // public:
    //     PerspectiveCamera(float fov = 90.0f, float near = 0.01f, float far = 1000.0f);
    //     ~PerspectiveCamera() = default;

    //     void SetPosition(glm::vec3 position)    { m_Position = position; UpdateView(); }
    //     void SetYaw(float yaw)                  { m_Yaw = yaw; UpdateView(); }
    //     void SetPitch(float pitch)              { m_Pitch = pitch; UpdateView(); }
    //     void SetRoll(float roll)                { m_Roll = roll; UpdateView(); }
        
    //     //void SetDirection(glm::vec3 direction)  { m_Direction = direction; UpdateView(); }
    //     //void SetUpVector(glm::vec3 upVector)    { m_Up = upVector; UpdateView(); }
    //     void SetFOV(float fov)                  { m_FOV = fov; UpdateProjection(); }
    //     void SetNearPlane(float near)           { m_Near = near; UpdateProjection(); }
    //     void SetFarPlane(float far)             { m_Far = far; UpdateProjection(); }

    //     float GetYaw() const { return m_Yaw; }
    //     float GetPitch() const { return m_Pitch; }
    //     float GetRoll() const { return m_Roll; }
    //     const glm::vec3& GetDirection() const { return m_Direction; }
    //     const glm::vec3& GetUpVector() const { return m_Up; }
    //     const glm::vec3& GetRightVector() const { return m_Right; }
    //     float GetFOV() const { return m_FOV; }
    //     float GetNearPlane() const { return m_Near; }
    //     float GetFarPlane() const { return m_Far; }
    // private:
    //     void UpdateView();
    //     void UpdateProjection();
    // private:
    //     glm::vec3 m_Up = glm::vec3(0.0f, 0.1f, 0.0f);
    //     glm::vec3 m_Right = glm::vec3(1.0f, 0.0f, 0.0f);
    //     float m_Yaw = 0.0f;
    //     float m_Pitch = 0.0f;
    //     float m_Roll = 0.0f;

    //     float m_FOV = glm::radians(45.0f);
    //     float m_Near = 0.01f;
    //     float m_Far = 1000.0f;
    // };

}