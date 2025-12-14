#pragma once
#include "Maths.h"
#include "Window.h"

class CollisionBox;

class Camera
{
public:
    Vec3 cameraPos;
    Vec3 cameraFront;
    Vec3 cameraRight;
    Vec3 worldUp;

    float yaw;
    float pitch;
    float mouseSensitivity;
    float moveSpeed;

    Camera()
    {
        cameraPos = Vec3(0, 5.0f, -15.0f);
        worldUp = Vec3(0, 1, 0);
        yaw = 90.0f;
        pitch = 0.0f;
        mouseSensitivity = 0.1f;
        moveSpeed = 8.0f;
        collisionBox = nullptr;

        updateVectors();
    }

    Matrix getViewMatrix()
    {
        return Matrix::lookAt(cameraPos, cameraPos + cameraFront, worldUp);
    }

    void update(float dt, Window* window);
    
    void setCollisionBox(CollisionBox* collisionBox);

private:
    CollisionBox* collisionBox;
    void updateVectors()
    {
        float radYaw = yaw * (3.14159f / 180.0f);
        float radPitch = pitch * (3.14159f / 180.0f);

        Vec3 front;
        front.x = cosf(radYaw) * cosf(radPitch);
        front.y = sinf(radPitch);
        front.z = sinf(radYaw) * cosf(radPitch);

        cameraFront = front.normalize();
        cameraRight = Cross(worldUp, cameraFront).normalize(); //
    }
};