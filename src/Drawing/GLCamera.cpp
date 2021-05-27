#include "GLCamera.h"

#include <glm/ext.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

namespace
{
static const glm::vec3 g_UpVector = glm::vec3(0, 1, 0);
}

glm::mat4 GLCamera::GetProjectionMatrix() const
{
    return glm::perspective(glm::radians(m_fovDeg), m_aspectRatio, m_minClipDistance, m_maxClipDistance);
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
    glm::vec3 rotationZ = m_normal;
    glm::vec3 rotationX = glm::normalize(glm::cross(g_UpVector, rotationZ));
    glm::vec3 rotationY = glm::normalize(glm::cross(rotationZ, rotationX));
    // clang-format off
    glm::mat3 rotMatrix(
        rotationX.x, rotationY.x, rotationZ.x,
        rotationX.y, rotationY.y, rotationZ.y,
        rotationX.z, rotationY.z, rotationZ.z);
    // clang-format on

    glm::vec3 localMove = by * rotMatrix;

    m_position += localMove;
}

void GLCamera::Rotate(const glm::vec3& axis, float rotDeg)
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

    glm::vec3 localAxis = axis * rotMatrix; 

    m_normal = glm::rotate(m_normal, glm::radians(rotDeg), localAxis);

    // todo : impl me
    // assert(!"Not implemented!");
}
