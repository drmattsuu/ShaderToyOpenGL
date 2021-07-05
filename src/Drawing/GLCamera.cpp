#include "GLCamera.h"

#include "GLInputManager.h"

#include <glm/ext.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "imgui.h"

namespace
{
static const glm::vec3 g_UpVector = glm::vec3(0, 1, 0);
}

GLCamera::GLCamera()
{
    m_keyEventDelegate = GLInputManager::GetInputManager()->SubscribeEvent(EventType::Key);
    m_keyEventDelegate->bind(this, &GLCamera::HandleKeyEvent);
    m_mouseButtonEventDelegate = GLInputManager::GetInputManager()->SubscribeEvent(EventType::MouseButton);
    m_mouseButtonEventDelegate->bind(this, &GLCamera::HandleMouseEvent);
    m_mouseMoveEventDelegate = GLInputManager::GetInputManager()->SubscribeEvent(EventType::MouseMove);
    m_mouseMoveEventDelegate->bind(this, &GLCamera::HandleMouseEvent);
}

GLCamera::~GLCamera()
{
    GLInputManager::GetInputManager()->UnsubscribeEvent(EventType::Key, m_keyEventDelegate);
    GLInputManager::GetInputManager()->UnsubscribeEvent(EventType::MouseButton, m_mouseButtonEventDelegate);
    GLInputManager::GetInputManager()->UnsubscribeEvent(EventType::MouseMove, m_mouseMoveEventDelegate);
}

void GLCamera::Update(float deltaT)
{
    glm::vec3 move(0.f);
    if (m_wasd[0])
        move.z += 1.f;
    if (m_wasd[1])
        move.z -= 1.f;
    if (m_wasd[2])
        move.x += 1.f;
    if (m_wasd[3])
        move.x -= 1.f;

    if (move != glm::vec3(0.0f))
    {
        move = glm::normalize(move);

        // move 1m per second
        move *= (deltaT * 0.001f);
        if (m_shift)
            move *= 5.f;

        MoveLocal(move);
    }

    if (!ImGui::GetIO().WantCaptureMouse && m_mouseButton && m_mouseDelta != glm::vec2(0.f))
    {
        float yScalar = 1 / m_aspectRatio;
        glm::vec2 rotVal(m_mouseDelta.y * yScalar, -m_mouseDelta.x);
        glm::vec2 rotDir = glm::normalize(rotVal);

        float deg = glm::length(rotVal) * 0.1f;
        Rotate(glm::vec3(rotDir.x, rotDir.y, 0.0f), deg);
    }
    
    m_mouseDelta.x = 0.f;
    m_mouseDelta.y = 0.f;
}

glm::mat4 GLCamera::GetViewMatrix() const
{
    const glm::vec3& f = m_normal;
    const glm::vec3 s(glm::normalize(glm::cross(f, g_UpVector)));
    const glm::vec3 u(glm::cross(s, f));

    glm::mat4 result(1.f);
    result[0][0] = s.x;
    result[1][0] = s.y;
    result[2][0] = s.z;
    result[0][1] = u.x;
    result[1][1] = u.y;
    result[2][1] = u.z;
    result[0][2] = -f.x;
    result[1][2] = -f.y;
    result[2][2] = -f.z;
    result[3][0] = -glm::dot(s, m_position);
    result[3][1] = -glm::dot(u, m_position);
    result[3][2] = glm::dot(f, m_position);

    return result;
}

void GLCamera::LookAt(const glm::vec3& point)
{
    m_normal = glm::normalize(point - m_position);
}

void GLCamera::MoveLocal(const glm::vec3& by)
{
    glm::mat3 rotMatrix = GetCurrentRotationMatrix();

    glm::vec3 localMove = by * rotMatrix;

    m_position += localMove;
}

void GLCamera::Rotate(const glm::vec3& axis, float rotDeg)
{
    glm::mat3 rotMatrix = GetCurrentRotationMatrix();

    glm::vec3 localAxis = axis * rotMatrix;

    m_normal = glm::rotate(m_normal, glm::radians(rotDeg), localAxis);
}

void GLCamera::SetAspect(float aspect)
{
    m_aspectRatio = aspect;
    m_perspective = glm::perspective(glm::radians(m_fovDeg), m_aspectRatio, m_minClipDistance, m_maxClipDistance);
}

void GLCamera::SetFOV(float fovDeg)
{
    m_fovDeg = fovDeg;
    m_perspective = glm::perspective(glm::radians(m_fovDeg), m_aspectRatio, m_minClipDistance, m_maxClipDistance);
}

bool GLCamera::HandleKeyEvent(EventPtr event)
{
    auto buttonEvent = dynamic_cast<ButtonEvent*>(event.get());

    if (buttonEvent)
    {
        bool pressed = buttonEvent->Action != GLFW_RELEASE;
        int key = buttonEvent->Button;
        switch (key)
        {
            case GLFW_KEY_LEFT_SHIFT:
                m_shift = pressed;
                break;
            case GLFW_KEY_W:
                m_wasd[0] = pressed;
                break;
            case GLFW_KEY_S:
                m_wasd[1] = pressed;
                break;
            case GLFW_KEY_A:
                m_wasd[2] = pressed;
                break;
            case GLFW_KEY_D:
                m_wasd[3] = pressed;
                break;
            default:
                return false;
        }
        // event handled
        return true;
    }
    return false;
}

bool GLCamera::HandleMouseEvent(EventPtr event)
{
    auto buttonEvent = dynamic_cast<ButtonEvent*>(event.get());
    if (buttonEvent)
    {
        bool pressed = buttonEvent->Action != GLFW_RELEASE;
        int button = buttonEvent->Button;
        if (button == GLFW_MOUSE_BUTTON_1)
        {
            m_mouseButton = pressed;
            return true;
        }
        return false;
    }

    auto moveEvent = dynamic_cast<MouseXYEvent*>(event.get());
    if (moveEvent)
    {
        m_mouseDelta.x = moveEvent->X;
        m_mouseDelta.y = moveEvent->Y;
    }

    return false;
}

glm::mat3 GLCamera::GetCurrentRotationMatrix()
{
    glm::vec3 rotationZ = m_normal;
    glm::vec3 rotationX = glm::normalize(glm::cross(g_UpVector, rotationZ));
    glm::vec3 rotationY = glm::normalize(glm::cross(rotationZ, rotationX));
    // clang-format off
    glm::mat3 rotMatrix(
        rotationX.x, rotationY.x, rotationZ.x,
        rotationX.y, rotationY.y, rotationZ.y,
        rotationX.z, rotationY.z, rotationZ.z);
    // clang-format on
    return rotMatrix;
}
