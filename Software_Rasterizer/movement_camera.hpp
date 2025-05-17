#pragma once

#include <Engine/engine.hpp>

enum camera_movement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Default camera values
static inline const rnd::f32 YAW = -90.0f;
static inline const rnd::f32 PITCH = 0.0f;
static inline const rnd::f32 SPEED = 2.5f;
static inline const rnd::f32 SENSITIVITY = 0.0001f;
static inline const rnd::f32 ZOOM = 45.0f;

class movement_camera
{
public:
    // camera Attributes
    math::vec3 Position;
    math::vec3 Front;
    math::vec3 Up;
    math::vec3 Right;
    math::vec3 WorldUp;
    // euler Angles
    rnd::f32 Yaw;
    rnd::f32 Pitch;
    // camera options
    rnd::f32 MovementSpeed;
    rnd::f32 MouseSensitivity;
    rnd::f32 Zoom;

    movement_camera(math::vec3 position = math::vec3(0.0f, 0.0f, -100.0f), math::vec3 up = math::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) 
        : 
        Front(math::vec3(0.0f, 0.0f, -1.0f)),
        MovementSpeed(SPEED),
        MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        update_camera_vectors();
    }
    // constructor with scalar values
    movement_camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) 
        :
        Front(math::vec3(0.0f, 0.0f, -1.0f)),
        MovementSpeed(SPEED), 
        MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = math::vec3(posX, posY, posZ);
        WorldUp = math::vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        update_camera_vectors();
    }

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    math::mat4 get_view_matrix()
    {
        return math::mat4::look_at(Position, Position + Front, Up);
    }


    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void process_keyboard(camera_movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
        if (direction == FORWARD)
            Position = Position + Front * velocity;
        if (direction == BACKWARD)
            Position = Position - Front * velocity;
        if (direction == LEFT)
            Position = Position - Right * velocity;
        if (direction == RIGHT)
            Position = Position + Right * velocity;
    }

    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void process_mouse_movement(float xoffset, float yoffset, bool constrainPitch = true)
    {
        xoffset *= SENSITIVITY;
        yoffset *= SENSITIVITY;

        Yaw += xoffset;
        Pitch += yoffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        // update Front, Right and Up Vectors using the updated Euler angles
        update_camera_vectors();
    }

private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    void update_camera_vectors()
    {
        // calculate the new Front vector
        math::vec3 front;
        front.x = cos(math::radians(Yaw)) * cos(math::radians(Pitch));
        front.y = sin(math::radians(Pitch));
        front.z = sin(math::radians(Yaw)) * cos(math::radians(Pitch));
        Front = math::normalize(front);
        // also re-calculate the Right and Up vector
        Right = math::normalize(math::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        Up = math::normalize(math::cross(Right, Front));
    }
};