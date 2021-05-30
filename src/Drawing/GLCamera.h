#pragma once

#include "GLInputManager.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

class GLCamera
{
public:
    GLCamera();
    ~GLCamera();

    void Update(float deltaT);

    const glm::mat4& GetProjectionMatrix() const { return m_perspective; }

    glm::mat4 GetViewMatrix() const;

    void LookAt(const glm::vec3& point);
    void MoveLocal(const glm::vec3& by);
    void Rotate(const glm::vec3& axis, float rotDeg);

    void SetPosition(const glm::vec3& pos) { m_position = pos; }
    const glm::vec3& GetPosition() const { return m_position; }
    void SetAspect(float aspect);
    float GetAspect() const { return m_aspectRatio; }
    void SetFOV(float fovDeg);
    float GetFOV() const { return m_fovDeg; }
    
    bool HandleKeyEvent(EventPtr event);
    bool HandleMouseEvent(EventPtr event);

private:
    glm::mat3 GetCurrentRotationMatrix();

private:
    glm::vec3 m_position{0.f};
    glm::vec3 m_normal{0.0f, 0.0f, 1.0f};
    glm::mat4 m_perspective{1.f};
    float m_fovDeg = 45.f;
    float m_aspectRatio = 1.33f;
    float m_minClipDistance = 0.1f;
    float m_maxClipDistance = 1000.f;
    bool m_recalculatePerspective = true;

    bool m_wasd[4]{false, false, false, false};
    bool m_shift = false;
    bool m_mouseButton{false};
    glm::vec2 m_mouseDelta{0.f};
    
    EventDelegatePtr m_keyEventDelegate;
    EventDelegatePtr m_mouseButtonEventDelegate;
    EventDelegatePtr m_mouseMoveEventDelegate;
};
