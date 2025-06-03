#include "Floor.h"

Floor::Floor() : size(200.0f), height(0.0f) {
    // Create rigid body for the floor
    body = new cyclone::RigidBody();

    // Set floor properties
    body->setInverseMass(0); // Static body (infinite mass)
    body->setInverseInertiaTensor(cyclone::Matrix3(0, 0, 0, 0, 0, 0, 0, 0, 0));
    body->setPosition(cyclone::Vector3(0, height, 0)); // Floor is at y=0
    body->setOrientation(cyclone::Quaternion(1, 0, 0, 0)); // No rotation
}

Floor::~Floor() { delete body; }

void Floor::setupFloor() {
    // Set up floor material properties
    GLfloat floorColor[] = {0.5f, 0.5f, 0.5f, 1.0f}; // Gray color
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, floorColor);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0.0f);
}

void Floor::draw() {
    glPushMatrix();

    // Get the transform matrix from the rigid body
    float transform[16];
    body->getGLTransform(transform);
    glMultMatrixf(transform);

    // Draw the floor as a large quad
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f); // Normal pointing up
    glVertex3f(-size / 2, 0.0f, -size / 2);
    glVertex3f(-size / 2, 0.0f, size / 2);
    glVertex3f(size / 2, 0.0f, size / 2);
    glVertex3f(size / 2, 0.0f, -size / 2);
    glEnd();

    glPopMatrix();
}

void Floor::setPosition(const cyclone::Vector3 &pos) {
    cyclone::Vector3 newPos = pos;
    newPos.y = height; // Keep floor at its fixed height
    body->setPosition(newPos);
}
