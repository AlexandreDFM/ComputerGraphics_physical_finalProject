#include "SimplePhysics.h"

#include <iostream>
#include <random>

void SimplePhysics::reset() {
    // Random number generator for box sizes and positions
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> sizeDist(0.5f, 2.0f);
    std::uniform_real_distribution<float> posDist(-5.0f, 5.0f);
    std::uniform_real_distribution<float> heightDist(1.0f, 5.0f);

    // Initialize 5 boxes with random sizes and positions
    for (int i = 0; i < 5; i++) {
        cyclone::Vector3 extents(sizeDist(gen), sizeDist(gen), sizeDist(gen));
        cyclone::Vector3 position(posDist(gen), heightDist(gen), posDist(gen));
        cyclone::Quaternion orientation;
        // orientation.setEuler(0, 0, 0); // No initial rotation

        boxData[i].setState(
            position,
            orientation,
            extents,
            cyclone::Vector3(0, 0, 0) // No initial velocity
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
    for (Box* box = boxData; box < boxData + 5; box++) {
        // Check for collisions with the ground plane
        if (!cData->hasMoreContacts()) return;
        cyclone::CollisionDetector::boxAndHalfSpace(*box, plane, cData);

        // Check for collisions with each other box
        for (Box* other = box + 1; other < boxData + 5; other++) {
            if (!cData->hasMoreContacts()) return;
            cyclone::CollisionDetector::boxAndBox(*box, *other, cData);
        }
    }
}

void SimplePhysics::update(cyclone::real duration) {
    // Generate contacts
    generateContacts();

    // Resolve the contacts
    resolver->resolveContacts(cData->contactArray, cData->contactCount, duration);

    // Update the physics of each box
    for (Box* box = boxData; box < boxData + 5; box++) {
        box->body->integrate(duration);
        box->calculateInternals();
    }
}

void SimplePhysics::render(int shadow) {
    // Draw each box
    for (Box* box = boxData; box < boxData + 5; box++) {
        std::cout << "Drawing PlayerCube at position: (" << box->getPosition().x << ", " << box->getPosition().y << ", " << box->getPosition().z << ")" << std::endl;
        box->draw(shadow);
    }
}
