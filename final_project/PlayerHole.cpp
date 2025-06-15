#include "PlayerCube.h"
#include <GL/glut.h>

PlayerCube::PlayerCube() :
    swallowRadius(7.0f), moveSpeed(10.0f), moveForward(false), moveBackward(false), moveLeft(false), moveRight(false),
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
    body->setPosition(cyclone::Vector3(0, 2.0f, 0)); // Start at a more reasonable height
    body->setVelocity(cyclone::Vector3(0, 0, 0));
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

    // Update position and velocity
    body->setPosition(newPos);
    body->setVelocity(velocity);
    body->calculateDerivedData(); // Ensure transform matrix is updated
}

void PlayerCube::draw() {
    // Get the transform matrix from the rigid body
    float transform[16];
    body->getGLTransform(transform);

    // Apply the transform to the modelview matrix
    glPushMatrix();
    glMultMatrixf(transform);

    // Draw a visual representation for the hole
    glColor4f(colorR, colorG, colorB, 0.8f);

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

void PlayerCube::setPosition(const cyclone::Vector3 &pos) {
    body->setPosition(pos);
    body->calculateDerivedData(); // Ensure transform matrix is updated
}

void PlayerCube::setColor(float r, float g, float b) {
    colorR = r;
    colorG = g;
    colorB = b;
}

void PlayerCube::checkSwallowObjects(std::vector<cyclone::RigidBody *> &objects) {
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