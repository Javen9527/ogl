// Created by jianbo
// Date: 2021/11/27

#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

/*
FPS style camera:

    Camera attributes:
    * position
    * three vector: front \ right \ up

    Actually you need three vector to define a camera:
    * position
    * front
    * world up
    
    Then you need to calculate the left camera attributes:
    * right = cross(front, world up), note: world up first then you will get -x
    * up = cross(right, front)
    
    WASD key was input for update m_postion vector.
    Mouse movement for Euler angle(yaw pitch row) was processed for m_front vector.
    Scroll is used for zoom, which will be the input for projection matrix
*/

namespace cam
{
 
// mouse direction
enum Camera_Movement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Default camera values
namespace attrib
{
    const float YAW = -90.0f;
    const float PITCH = 0.0f;

    const float SPEED = 2.5f;
    const float SENSITIVITY = 0.1f;
    const float ZOOM = 45.0f;
}

class Camera
{
public:
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = attrib::YAW, float pitch = attrib::PITCH) 
        : m_position(position)
        , m_front(glm::vec3(0.0f, 0.0f, -1.0f))
        , m_worldUp(worldUp)

        , m_yaw(yaw)
        , m_pitch(pitch)
        , m_movmentSpeed(attrib::SPEED)
        , m_mouseSensitivity(attrib::SENSITIVITY)
        , m_zoom(attrib::ZOOM)
    {
        updateCameraVectors();
    }
    
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) 
        : m_position(glm::vec3(posX, posY, posZ))
        , m_front(glm::vec3(0.0f, 0.0f, -1.0f))
        , m_worldUp(glm::vec3(upX, upY, upZ))

        , m_yaw(yaw)
        , m_pitch(pitch)
        , m_movmentSpeed(attrib::SPEED)
        , m_mouseSensitivity(attrib::SENSITIVITY)
        , m_zoom(attrib::ZOOM)
    {
        updateCameraVectors();
    }

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix() { return glm::lookAt(m_position, m_position + m_front, m_worldUp/*m_up*/);}
    float GetZoomLevel() const { return m_zoom; }

    // processes input
    void ProcessKeyboard(const Camera_Movement direction, const float deltaTime);
    void ProcessMouseMovement(const float xoffset, const float yoffset, GLboolean constrainPitch = true);
    void ProcessMouseScroll(const float yoffset);

private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors();

private:
    // camera Attributes
    glm::vec3 m_position;
    glm::vec3 m_front;
    glm::vec3 m_worldUp;

    glm::vec3 m_up;
    glm::vec3 m_right;

    // euler Angles for front
    float m_yaw;
    float m_pitch;

    // camera options
    float m_movmentSpeed;
    float m_mouseSensitivity;
    float m_zoom; // for projection matrix
};

//////////////////// IMPLEMENTATION ////////////////////

inline void Camera::ProcessKeyboard (const Camera_Movement direction, const float deltaTime)
{
    float velocity = m_movmentSpeed * deltaTime;
    switch (direction)
    {
    case FORWARD:
        m_position += m_front * velocity;
        break;
    case BACKWARD:
        m_position -= m_front * velocity;
        break;
    case LEFT:
        m_position -= m_right * velocity;
        break;
    case RIGHT:
        m_position += m_right * velocity;
        break;
    default:
        break;
    }
}

inline void Camera::ProcessMouseMovement(const float xoffset, const float yoffset, GLboolean constrainPitch)
{
    m_yaw   += xoffset * m_mouseSensitivity;
    m_pitch += yoffset * m_mouseSensitivity;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch)
    {
        if (m_pitch > 89.0f)
            m_pitch = 89.0f;
        if (m_pitch < -89.0f)
            m_pitch = -89.0f;
    }

    // update m_front, m_right and m_up Vectors using the updated Euler angles
    updateCameraVectors();
}

inline void Camera::ProcessMouseScroll(const float yoffset)
{
    m_zoom -= (float)yoffset;

    if (m_zoom < 1.0f)
        m_zoom = 1.0f;
    if (m_zoom > 90.0f)
        m_zoom = 90.0f;
}

inline void Camera::updateCameraVectors()
{
    // calculate the new m_front vector
    glm::vec3 front;
    front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    front.y = sin(glm::radians(m_pitch));
    front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    m_front = glm::normalize(front);

    // re-calculate the m_right and m_up vector
    m_right = glm::normalize(glm::cross(m_front, m_worldUp)); 
    m_up    = glm::normalize(glm::cross(m_right, m_front));
}
   
} // namespace cam
