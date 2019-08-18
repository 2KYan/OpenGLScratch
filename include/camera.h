#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>

#include <vector>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;
const float ASPECTRATIO = 1.0f;
const float ARCBALL_SIZE = 100.0f;


// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
    // Camera Attributes for Reset
    glm::vec3 _Position;
    glm::vec3 _WorldUp;
    // Camera Attributes
    glm::vec3 Position;
    glm::vec3 WorldUp;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    // Euler Angles
    float Yaw;
    float Pitch;
    // Camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;
    float AspectRatio;
    float Size;

    glm::mat4 Model = glm::mat4(1.0f);
    glm::mat4 View;
    glm::mat4 Projection;

    // Constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float aspectRatio = ASPECTRATIO, float yaw = YAW, float pitch = PITCH) 
        : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM), Size(ARCBALL_SIZE)
    {
        _Position = Position = position;
        _WorldUp = WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        AspectRatio = aspectRatio;
        updateCameraVectors();
    }
    // Constructor with scalar values
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) 
        : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM), Size(ARCBALL_SIZE)
    {
        _Position = Position = glm::vec3(posX, posY, posZ);
        _WorldUp = WorldUp = glm::vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    void reset()
    {
        Position = _Position;
        WorldUp = _WorldUp;
        updateCameraVectors();
    }

    void updateAspectRatio(float ar)
    {
        AspectRatio = ar;
    }

    glm::mat4 GetModelMatrix()
    {
        return Model;
    }

    // Returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix()
    {
        return View;
    }

    glm::mat4 GetProjectionMatrix()
    {
        return Projection;
    }

    // Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
        if (direction == FORWARD)
            Position += Front * velocity;
        if (direction == BACKWARD)
            Position -= Front * velocity;
        if (direction == LEFT)
            Position -= Right * velocity;
        if (direction == RIGHT)
            Position += Right * velocity;

        updateCameraVectors();
    }

    // Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw += xoffset;
        Pitch += yoffset;

        // Make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch) {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        // Update Front, Right and Up Vectors using the updated Euler angles
        updateCameraVectors();
    }

    // Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset)
    {
        if (Zoom >= 1.0f && Zoom <= 45.0f)
            Zoom -= yoffset;
        if (Zoom <= 1.0f)
            Zoom = 1.0f;
        if (Zoom >= 45.0f)
            Zoom = 45.0f;

        updateCameraVectors();
    }
        
   void translate(int32_t xrel, int32_t yrel)
    {
       return;
    }

    void Rotate(int32_t x, int32_t y, int32_t xrel, int32_t yrel, int32_t width, int32_t height)
    {
        //x, y is the new position;
        int32_t old_x = x - xrel;
        int32_t old_y = y - yrel;

        if (xrel == 0 && yrel == 0) return;

        glm::vec2 old_pos = scale2snorm(old_x, old_y, width, height);
        glm::vec2 new_pos = scale2snorm(x, y, width, height);
        glm::vec3 p1 = project2Spehere(old_pos);
        glm::vec3 p2 = project2Spehere(new_pos);

        //std::cout << glm::to_string(p1)  << std::endl;
        //std::cout << glm::to_string(p2)  << std::endl;

        glm::vec3 r_axis = glm::cross(p1, p2);
        glm::mat3 camera2object = glm::inverse(glm::mat3(View) * glm::mat3(Model));
        glm::vec3 r_axis_in_object = camera2object * r_axis;
        float r_angle = 180 * glm::length(p1 - p2);
        float angle = acos(glm::min(1.0f, glm::dot(p1, p2)));
        Model = glm::rotate(Model, r_angle, r_axis_in_object);

        return;
    }

private:
    // Calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors()
    {
        // Calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        // Also re-calculate the Right and Up vector
        Right = glm::normalize(glm::cross(Front, WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        Up = glm::normalize(glm::cross(Right, Front));

        updateMatrix();
    }

    void updateMatrix()
    {
        View = glm::lookAt(Position, Position + Front, Up);
        Projection = glm::perspective(glm::radians(Zoom), AspectRatio, 0.1f, 100.0f);
    }

    glm::vec2 scale2snorm(int32_t x, int32_t y, int32_t w, int32_t h)
    {
        return glm::vec2(x * 2.0 / w - 1.0f, y * 2.0f / h - 1.0f);
    }

    glm::vec3 project2Spehere(glm::vec2 xy)
    {
        static const float sqrt2 = sqrt(2.0f);
        glm::vec3 result;

        float d = glm::length(xy);
        if (d < Size * sqrt2 / 2.0f) {
            result.z = sqrt(Size * Size - d * d);
        } else {
            result.z = Size * Size / (2 * d);
        }
        result.x = xy.x;
        result.y = xy.y;

        return glm::normalize(result);
    }

};
#endif
