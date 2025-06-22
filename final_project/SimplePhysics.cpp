#include "SimplePhysics.h"

#include <iostream>
#include <random>

void SimplePhysics::reset() {
    // Random number generator for box sizes and positions
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> sizeDist(0.5f, 2.0f);
    std::uniform_real_distribution<float> posDist(-100.0f, 100.0f);
    std::uniform_real_distribution<float> heightDist(1.0f, 5.0f);

    for (auto box: boxData) {
        cyclone::Vector3 extents(sizeDist(gen), sizeDist(gen), sizeDist(gen));
        cyclone::Vector3 position(posDist(gen), heightDist(gen), posDist(gen));
        cyclone::Quaternion orientation;

        box->setState(position, orientation, extents, cyclone::Vector3(0, 0, 0) // No initial velocity
        );
    }
}

void SimplePhysics::generateContacts() {
    // Set up the collision data structure
    cData->reset(maxContacts);
    cData->friction = 0.1f;
    cData->restitution = 0.6f;
    cData->tolerance = 0.1f;

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
            cyclone::CollisionDetector::boxAndHalfSpace(*box, plane, cData);
        }

        // Check for collisions with each other box
        // for (auto other: boxData) {
        //     if (!other->isValid())
        //         continue;
        //     if (!cData->hasMoreContacts())
        //         return;
        //     cyclone::CollisionDetector::boxAndBox(*box, *other, cData);
        // }
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

void SimplePhysics::render(int shadow) {
    for (auto box: boxData) {
        if (box->isValid()) {
            box->draw(shadow);
        }
    }
}
