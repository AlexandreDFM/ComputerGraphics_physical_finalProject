#include "Hole.h"
#include <GL/glut.h>
#include <iostream>

Hole::Hole() :
    swallowRadius(7.0f), moveSpeed(10.0f), moveForward(false), moveBackward(false), moveLeft(false), moveRight(false) {
    // Create rigid body for the hole
    body = new cyclone::RigidBody();

    // Set hole properties
    body->setInverseMass(0); // Infinite mass so it's not affected by forces
    body->setInverseInertiaTensor(cyclone::Matrix3(0, 0, 0, 0, 0, 0, 0, 0, 0));
    body->setDamping(0.9, 0.9);
    body->setAcceleration(cyclone::Vector3::GRAVITY * 0); // No gravity
    body->setPosition(cyclone::Vector3(0, 0.1, 0)); // Set initial position at fixed height
    body->setVelocity(cyclone::Vector3(0, 0, 0)); // Initialize velocity to zero
}

Hole::~Hole() { delete body; }

void Hole::setMovement(bool forward, bool backward, bool left, bool right) {
    moveForward = forward;
    moveBackward = backward;
    moveLeft = left;
    moveRight = right;
}

void Hole::update(float duration) {
    // Debug output for movement flags
    std::cout << "Hole movement flags - Forward: " << moveForward << " Backward: " << moveBackward
              << " Left: " << moveLeft << " Right: " << moveRight << std::endl;

    // Calculate velocity based on movement flags
    cyclone::Vector3 velocity(0, 0, 0);

    // Normalize diagonal movement
    if (moveForward && moveLeft) {
        velocity.x = -moveSpeed * 0.7071f; // cos(45 degrees)
        velocity.z = moveSpeed * 0.7071f; // sin(45 degrees)
        std::cout << "Moving diagonally forward-left" << std::endl;
    } else if (moveForward && moveRight) {
        velocity.x = moveSpeed * 0.7071f;
        velocity.z = moveSpeed * 0.7071f;
        std::cout << "Moving diagonally forward-right" << std::endl;
    } else if (moveBackward && moveLeft) {
        velocity.x = -moveSpeed * 0.7071f;
        velocity.z = -moveSpeed * 0.7071f;
        std::cout << "Moving diagonally backward-left" << std::endl;
    } else if (moveBackward && moveRight) {
        velocity.x = moveSpeed * 0.7071f;
        velocity.z = -moveSpeed * 0.7071f;
        std::cout << "Moving diagonally backward-right" << std::endl;
    } else {
        if (moveForward) {
            velocity.z = moveSpeed;
            std::cout << "Moving forward" << std::endl;
        }
        if (moveBackward) {
            velocity.z = -moveSpeed;
            std::cout << "Moving backward" << std::endl;
        }
        if (moveLeft) {
            velocity.x = -moveSpeed;
            std::cout << "Moving left" << std::endl;
        }
        if (moveRight) {
            velocity.x = moveSpeed;
            std::cout << "Moving right" << std::endl;
        }
    }

    // Debug output for calculated velocity
    std::cout << "Calculated velocity - X: " << velocity.x << " Y: " << velocity.y << " Z: " << velocity.z << std::endl;

    // Update position
    cyclone::Vector3 currentPos = body->getPosition();
    cyclone::Vector3 newPos = currentPos + velocity * duration;
    newPos.y = 0.1; // Keep fixed height

    // Debug output for position update
    std::cout << "Position update - Current: (" << currentPos.x << ", " << currentPos.y << ", " << currentPos.z
              << ") New: (" << newPos.x << ", " << newPos.y << ", " << newPos.z << ")" << std::endl;

    // Update position and velocity
    body->setPosition(newPos);
    body->setVelocity(velocity);
}

void Hole::draw() {
    // Get the transform matrix from the rigid body
    float transform[16];
    body->getGLTransform(transform);

    // Apply the transform to the modelview matrix
    glPushMatrix();
    glMultMatrixf(transform);

    // Draw a visual representation for the hole
    glColor4f(0.0f, 0.0f, 0.0f, 0.8f); // More opaque black for better visibility

    // Draw a disc on the ground plane
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0.0f, 0.0f, 0.0f); // Center at ground level
    int segments = 30;
    for (int i = 0; i <= segments; ++i) {
        float angle = 2.0f * static_cast<float>(M_PI) * float(i) / float(segments);
        glVertex3f(std::cos(angle) * swallowRadius, 0.0f, std::sin(angle) * swallowRadius);
    }
    glEnd();

    // Draw a small indicator sphere at the hole's center
    glColor4f(1.0f, 0.0f, 0.0f, 1.0f); // Red color
    glutSolidSphere(0.2f, 10, 10); // Small sphere to mark the center

    glPopMatrix();
}

void Hole::setPosition(const cyclone::Vector3 &pos) {
    cyclone::Vector3 newPos = pos;
    newPos.y = 0.1; // Keep fixed height
    body->setPosition(newPos);
}

void Hole::checkSwallowObjects(std::vector<cyclone::RigidBody *> &objects) {
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
        float distance = displacement.magnitude();

        // Check if object is close to the hole
        if (distance < swallowRadius) {
            // If the object is within the swallowing radius, apply a force pulling it towards the hole
            cyclone::Vector3 pullDirection = displacement.unit();
            float pullForceMagnitude = (swallowRadius - distance) * 20.0f;
            cyclone::Vector3 pullForce = pullDirection * pullForceMagnitude;
            currentBody->addForce(pullForce);

            // Check if the object is very close to be considered swallowed
            if (distance < swallowRadius * 0.5f) {
                // Remove the swallowed object
                delete currentBody;
                it = objects.erase(it);

                // Increase hole size
                swallowRadius += 0.5f;

                continue;
            }
        }
        ++it;
    }
}
