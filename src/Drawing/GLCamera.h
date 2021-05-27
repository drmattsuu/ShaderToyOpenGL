#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

class GLCamera
{
public:
    GLCamera() = default;
    ~GLCamera() = default;

    glm::mat4 GetProjectionMatrix() const;
    glm::mat4 GetViewMatrix() const;

    void LookAt(const glm::vec3& point);
    void MoveLocal(const glm::vec3& by);
    void Rotate(const glm::vec3& axis, float rotDeg);
    
    void SetPosition(const glm::vec3& pos) { m_position = pos; }
    const glm::vec3& GetPosition() const { return m_position; }
    void SetAspect(float aspect) { m_aspectRatio = aspect; }
    float GetAspect() const { return m_aspectRatio; }
    void SetFOV(float fovDeg) { m_fovDeg = fovDeg; }
    float getFOV() const { return m_fovDeg; }

private:
    glm::vec3 m_position{0.f};
    glm::vec3 m_normal{0.0f, 0.0f, 1.0f};
    float m_fovDeg = 45.f;
    float m_aspectRatio = 1.33f;
    float m_minClipDistance = 0.1f;
    float m_maxClipDistance = 1000.f;
};
