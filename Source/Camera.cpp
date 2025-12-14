#include "Camera.h"
#include "Collision.h"

void Camera::setCollisionBox(CollisionBox* box)
{
	collisionBox = box;
}

void Camera::update(float dt, Window* window)
{
    int centerX = window->width / 2;
    int centerY = window->height / 2;

    float xOffset = (float)(centerX - window->mousex);
    float yOffset = (float)(centerY - window->mousey);

    xOffset *= mouseSensitivity;
    yOffset *= mouseSensitivity;

    yaw += xOffset;
    pitch += yOffset;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    POINT centerPt;
    centerPt.x = centerX;
    centerPt.y = centerY;
    ClientToScreen(window->hwnd, &centerPt);
    SetCursorPos(centerPt.x, centerPt.y);

    window->updateMouse(centerX, centerY);

    updateVectors();

    float speed = dt * moveSpeed;

    Vec3 moveDir = cameraFront;
    moveDir.y = 0;
    moveDir = moveDir.normalize();

    Vec3 moveDelta(0, 0, 0);
    if (window->keys['W']) moveDelta = moveDelta + moveDir * speed;
    if (window->keys['S']) moveDelta = moveDelta - moveDir * speed;
    if (window->keys['A']) moveDelta = moveDelta - cameraRight * speed;
    if (window->keys['D']) moveDelta = moveDelta + cameraRight * speed;
    
    if (moveDelta.lengthSq() > 0.0001f)
    {
        Vec3 newPos = cameraPos + moveDelta;
        
        if (collisionBox != nullptr && collisionBox->checkCollision(newPos))
        {
            return;
        }
        
        cameraPos = newPos;
    }
}