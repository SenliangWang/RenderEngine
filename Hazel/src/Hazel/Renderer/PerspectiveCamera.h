#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Hazel {

    class PerspectiveCamera
    {
    public:
        PerspectiveCamera(float fov, float aspectRatio, float nearClip, float farClip)
            : m_FOV(fov), m_AspectRatio(aspectRatio), m_NearClip(nearClip), m_FarClip(farClip)
        {
            RecalculateProjectionMatrix();
            RecalculateViewMatrix();
        }

        const glm::vec3& GetPosition() const { return m_Position; }
        void SetPosition(const glm::vec3& position) 
        { 
            m_Position = position; 
            RecalculateViewMatrix(); 
        }

        void LookAt(const glm::vec3& target)
        {
            m_ViewMatrix = glm::lookAt(m_Position, target, glm::vec3(0.0f, 1.0f, 0.0f));
            m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
        }

        const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
        const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
        const glm::mat4& GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }

        void SetProjection(float fov, float aspectRatio, float nearClip, float farClip)
        {
            m_FOV = fov;
            m_AspectRatio = aspectRatio;
            m_NearClip = nearClip;
            m_FarClip = farClip;
            RecalculateProjectionMatrix();
        }

    private:
        void RecalculateProjectionMatrix()
        {
            m_ProjectionMatrix = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearClip, m_FarClip);
            m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
        }

        void RecalculateViewMatrix()
        {
            m_ViewMatrix = glm::lookAt(m_Position, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
        }

    private:
        glm::mat4 m_ProjectionMatrix;
        glm::mat4 m_ViewMatrix;
        glm::mat4 m_ViewProjectionMatrix;

        glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
        
        float m_FOV = 45.0f;
        float m_AspectRatio = 16.0f / 9.0f;
        float m_NearClip = 0.1f;
        float m_FarClip = 100.0f;
    };

}
