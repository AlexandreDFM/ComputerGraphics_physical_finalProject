/**
 * File Name: PlayerHole.cpp
 * Author: Alexandre Kévin DE FREITAS MARTINS
 * Creation Date: 15/6/2025
 * Description: This is the PlayerHole.cpp
 * Copyright (c) 2025 Alexandre Kévin DE FREITAS MARTINS
 * Version: 1.0.0
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the 'Software'), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "PlayerHole.h"
#include <windows.h>

PlayerHole::PlayerHole() :
    swallowRadius(5.0f), moveSpeed(10.0f), moveForward(false), moveBackward(false), moveLeft(false), moveRight(false),
    cubeSize(2.0f),
    colorR(1.0f), colorG(0.4f), colorB(0.7f) // Initial pink color
{
    // Create rigid body for the player cube
    body = new cyclone::RigidBody();

    // Set cube properties
    body->setInverseMass(0);
    body->setInverseInertiaTensor(cyclone::Matrix3(0, 0, 0, 0, 0, 0, 0, 0, 0));
    body->setDamping(0.9, 0.9);
    body->setAcceleration(cyclone::Vector3::GRAVITY * 0);
    body->setPosition(cyclone::Vector3(0, 0.1f, 0)); // Start at a more reasonable height
    body->setVelocity(cyclone::Vector3(0, 0, 0));
}

PlayerHole::~PlayerHole() { delete body; }

void PlayerHole::setMovement(bool forward, bool backward, bool left, bool right) {
    moveForward = forward;
    moveBackward = backward;
    moveLeft = left;
    moveRight = right;
}

void PlayerHole::update(float duration) {
    // Calculate velocity based on movement flags
    cyclone::Vector3 velocity(0, 0, 0);

    // Normalize diagonal movement
    if (moveForward && moveLeft) {
        velocity.x = -moveSpeed * 0.7071f;
        velocity.z = -moveSpeed * 0.7071f;
    } else if (moveForward && moveRight) {
        velocity.x = moveSpeed * 0.7071f;
        velocity.z = -moveSpeed * 0.7071f;
    } else if (moveBackward && moveLeft) {
        velocity.x = -moveSpeed * 0.7071f;
        velocity.z = moveSpeed * 0.7071f;
    } else if (moveBackward && moveRight) {
        velocity.x = moveSpeed * 0.7071f;
        velocity.z = moveSpeed * 0.7071f;
    } else {
        if (moveForward)
            velocity.z = -moveSpeed;
        if (moveBackward)
            velocity.z = moveSpeed;
        if (moveLeft)
            velocity.x = -moveSpeed;
        if (moveRight)
            velocity.x = moveSpeed;
    }

    // Update position
    cyclone::Vector3 currentPos = body->getPosition();
    cyclone::Vector3 newPos = currentPos + velocity * duration;

    // Clamp position to stay within [-100, 100] range in x and z
    // This ensures the player hole does not move out of bounds
    newPos.x = max(-100.0f + swallowRadius, min(100.0f - swallowRadius, newPos.x));
    newPos.z = max(-100.0f + swallowRadius, min(100.0f - swallowRadius, newPos.z));

    // Update position and velocity
    body->setPosition(newPos);
    body->setVelocity(velocity);
    body->calculateDerivedData(); // Ensure transform matrix is updated
}


void PlayerHole::draw(GLuint textureID) {
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glColor3f(1, 1, 1);

    // Transform
    float transform[16];
    body->getGLTransform(transform);
    glPushMatrix();
    glMultMatrixf(transform);

    // Draw textured disc
    glBegin(GL_TRIANGLE_FAN);
    // Center vertex (at UV 0.5,0.5)
    glNormal3f(0, 1, 0);
    glTexCoord2f(0.5f, 0.5f);
    glVertex3f(0.0f, 0.0f, 0.0f);

    const int segments = 30;
    for (int i = 0; i <= segments; ++i) {
        float angle = 2.0f * M_PI * float(i) / float(segments);
        float x = std::cos(angle) * swallowRadius;
        float z = std::sin(angle) * swallowRadius;

        // compute UV on circle
        float u = 0.5f + 0.5f * std::cos(angle);
        float v = 0.5f + 0.5f * std::sin(angle);

        glNormal3f(0, 1, 0);
        glTexCoord2f(u, v);
        glVertex3f(x, 0.0f, z);
    }
    glEnd();

    // Small white sphere at center
    glDisable(GL_TEXTURE_2D);
    glColor4f(1, 1, 1, 1);
    glutSolidSphere(0.2f, 10, 10);

    glPopMatrix();

    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
}


void PlayerHole::setPosition(const cyclone::Vector3 &pos) {
    body->setPosition(pos);
    body->calculateDerivedData(); // Ensure transform matrix is updated
}

void PlayerHole::setColor(float r, float g, float b) {
    colorR = r;
    colorG = g;
    colorB = b;
}

void PlayerHole::checkSwallowObjects(std::vector<cyclone::RigidBody *> &objects) {
    cyclone::Vector3 holePosition = body->getPosition();

    for (auto it = objects.begin(); it != objects.end();) {
        cyclone::RigidBody *currentBody = *it;

        // Don't check for swallowing with the hole itself
        if (currentBody == body) {
            ++it;
            continue;
        }

        cyclone::Vector3 objectPosition = currentBody->getPosition();
        cyclone::Vector3 displacement = objectPosition - holePosition;
        cyclone::real distance = displacement.magnitude();

        // Check if object is close to the hole
        if (distance < swallowRadius) {
            // If the object is within the swallowing radius, apply a force pulling it towards the hole
            cyclone::Vector3 pullDirection = displacement.unit();
            cyclone::real pullForceMagnitude = (swallowRadius - distance) * 20.0f;
            cyclone::Vector3 pullForce = pullDirection * pullForceMagnitude;
            pullForce.invert();
            currentBody->addForce(pullForce);
            

            if (distance < swallowRadius * 0.7f) {
                // Deactivate the floor for the box take normal gravity
                // currentBody->setDamping(0.9f, 0.9f);
                // currentBody->setAcceleration(cyclone::Vector3::GRAVITY * 0);
                // currentBody->setCanSleep(true);
                // currentBody->setAwake(true);
                // currentBody->setVelocity(cyclone::Vector3(0, 0, 0));
                currentBody->addRotation(pullForce * 0.01f);
                simplePhysics->setSwallowed(currentBody, true);
            }

            // Check if the object is very close to be considered swallowed
            if (distance < swallowRadius * 0.1f) {
                simplePhysics->removeBox(currentBody);
                it = objects.erase(it);
                //swallowRadius += 0.1f;
                score->addToScore(1);
                continue;
            }
        }

        if (objectPosition.y < -5.0f) {
            simplePhysics->removeBox(currentBody);
            it = objects.erase(it);
             //swallowRadius += 0.1f;
            score->addToScore(1);
            continue;
        }

        ++it;
    }
}