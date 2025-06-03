#include "PlayerCube.h"
#include <GL/glut.h>
#include <iostream>

PlayerCube::PlayerCube() :
    moveSpeed(10.0f), moveForward(false), moveBackward(false), moveLeft(false), moveRight(false),
    cubeSize(2.0f), // 2x2x2 cube
    colorR(1.0f), colorG(0.4f), colorB(0.7f) // Initial pink color
{
    // Create rigid body for the player cube
    body = new cyclone::RigidBody();

    // Set cube properties
    body->setInverseMass(0); // Infinite mass so it's not affected by forces
    body->setInverseInertiaTensor(cyclone::Matrix3(0, 0, 0, 0, 0, 0, 0, 0, 0));
    body->setDamping(0.9, 0.9);
    body->setAcceleration(cyclone::Vector3::GRAVITY * 0); // No gravity
    body->setPosition(cyclone::Vector3(0, cubeSize / 2, 0)); // Set initial position at half height (bottom on floor)
    body->setVelocity(cyclone::Vector3(0, 0, 0)); // Initialize velocity to zero
}

PlayerCube::~PlayerCube() { delete body; }

void PlayerCube::setMovement(bool forward, bool backward, bool left, bool right) {
    moveForward = forward;
    moveBackward = backward;
    moveLeft = left;
    moveRight = right;
}

void PlayerCube::update(float duration) {
    // Calculate velocity based on movement flags
    cyclone::Vector3 velocity(0, 0, 0);

    // Normalize diagonal movement
    if (moveForward && moveLeft) {
        velocity.x = -moveSpeed * 0.7071f; // cos(45 degrees)
        velocity.z = moveSpeed * 0.7071f; // sin(45 degrees)
    } else if (moveForward && moveRight) {
        velocity.x = moveSpeed * 0.7071f;
        velocity.z = moveSpeed * 0.7071f;
    } else if (moveBackward && moveLeft) {
        velocity.x = -moveSpeed * 0.7071f;
        velocity.z = -moveSpeed * 0.7071f;
    } else if (moveBackward && moveRight) {
        velocity.x = moveSpeed * 0.7071f;
        velocity.z = -moveSpeed * 0.7071f;
    } else {
        if (moveForward)
            velocity.z = moveSpeed;
        if (moveBackward)
            velocity.z = -moveSpeed;
        if (moveLeft)
            velocity.x = -moveSpeed;
        if (moveRight)
            velocity.x = moveSpeed;
    }

    // Update position
    cyclone::Vector3 currentPos = body->getPosition();
    cyclone::Vector3 newPos = currentPos + velocity * duration;
    newPos.y = cubeSize / 2; // Keep the cube's center at half height

    // Update position and velocity
    body->setPosition(newPos);
    body->setVelocity(velocity);
}

void PlayerCube::draw() {
    float transform[16];
    body->getGLTransform(transform);

    glPushMatrix();
    glMultMatrixf(transform);

    // Draw the cube
    glColor3f(colorR, colorG, colorB); // Use the stored color
    glutSolidCube(cubeSize);

    // Draw a small red sphere at the center to help with orientation
    glColor3f(1.0f, 0.0f, 0.0f); // Red color
    glutSolidSphere(0.2f, 10, 10);

    glPopMatrix();
}

void PlayerCube::setPosition(const cyclone::Vector3 &pos) {
    cyclone::Vector3 newPos = pos;
    newPos.y = cubeSize / 2; // Keep the cube's center at half height
    body->setPosition(newPos);
}

void PlayerCube::setColor(float r, float g, float b) {
    colorR = r;
    colorG = g;
    colorB = b;
    // Log when color changes
    std::cout << "PlayerCube color changed to: (" << colorR << ", " << colorG << ", " << colorB << ")" << std::endl;
}
