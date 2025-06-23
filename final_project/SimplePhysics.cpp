#include "SimplePhysics.h"

#include <iostream>
#include <random>

void SimplePhysics::reset() {
    // Random number generator for box sizes and positions
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> sizeDist(1.0f, 2.0f);
    std::uniform_real_distribution<float> posDist(-100.0f, 100.0f);
    std::uniform_real_distribution<float> heightDist(10.0f, 50.0f);

    for (auto box: boxData) {
        const float scale = sizeDist(gen);
        cyclone::Vector3 extents(1.0f, 2.0f, 1.0f);
        extents *= scale;
        cyclone::Vector3 position(posDist(gen), heightDist(gen), posDist(gen));
        cyclone::Quaternion orientation;

        box->setState(position, orientation, extents, cyclone::Vector3(0, 0, 0));
    }
}

void SimplePhysics::generateContacts() {
    // Set up the collision data structure
    cData->reset(maxContacts);
    cData->friction = 0.5f;  // Increased friction for better stability
    cData->restitution = 0.1f;  // Reduced restitution to minimize bouncing
    cData->tolerance = 0.05f;  // Increased tolerance to prevent micro-collisions

    // Create the ground plane
    cyclone::CollisionPlane plane;
    plane.direction = cyclone::Vector3(0, 1, 0);
    plane.offset = 0;

    // Check collisions with ground and between boxes
    for (auto box: boxData) {
        if (!box->isValid())
            continue;

        // Check for collisions with the ground plane
        if (!cData->hasMoreContacts())
            return;
        if (!box->isSwallowed()) {
            // Only generate contacts if the box is close to or below the ground
            cyclone::Vector3 position = box->getPosition();
            cyclone::Vector3 extents = box->halfSize;
            if (position.y - extents.y <= cData->tolerance) {
                cyclone::CollisionDetector::boxAndHalfSpace(*box, plane, cData);
            }
        }

         //Check for collisions with each other box
         //for (auto other: boxData) {
         //    if (!other->isValid())
         //        continue;
         //    if (!cData->hasMoreContacts())
         //        return;
         //    cyclone::CollisionDetector::boxAndBox(*box, *other, cData);
         //}
    }
}

void SimplePhysics::update(cyclone::real duration) {
    // Generate contacts
    generateContacts();

    // Resolve the contacts
    resolver->resolveContacts(cData->contactArray, cData->contactCount, duration);

    // Update the physics of each box
    for (auto box: boxData) {
        if (box->isValid()) {
            box->body->integrate(duration);
            box->calculateInternals();
        }
    }
}

void SimplePhysics::render(int shadow, const GLuint textureID) {
    for (int i = 0; i < boxData.size(); i++) {
        if (boxData[i]->isValid()) {
            boxData[i]->draw(i + 1, shadow, textureID);
            if (m_drawHitboxes) {
                boxData[i]->drawHitbox(i + 1, shadow);
            }
        }
    }
}
