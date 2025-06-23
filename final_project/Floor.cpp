#include "Floor.h"
#include <iostream>

Floor::Floor(int floorSize, float height) : size(floorSize), height(height) {
    // Create rigid body for the floor
    body = new cyclone::RigidBody();

    // Set floor properties
    body->setInverseMass(0); // Static body (infinite mass)
    body->setInverseInertiaTensor(cyclone::Matrix3());
    body->setPosition(cyclone::Vector3(0, height, 0)); // Floor is at y=0
    body->setOrientation(cyclone::Quaternion(1, 0, 0, 0)); // No rotation
}

Floor::~Floor() { delete body; }

void Floor::draw(GLuint textureID) {
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // just in case, reset color & env
    glColor3f(1, 1, 1);
    //glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glPushMatrix();

    float transform[16];
    body->getGLTransform(transform);
    glMultMatrixf(transform);

    const float tile = 10.0f;

    GLint bound = 0;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &bound);

    glBegin(GL_QUADS);
    glNormal3f(0, 1, 0);

    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-size / 2, height, -size / 2);
    glTexCoord2f(0.0f, tile);
    glVertex3f(-size / 2, height, size / 2);
    glTexCoord2f(tile, tile);
    glVertex3f(size / 2, height, size / 2);
    glTexCoord2f(tile, 0.0f);
    glVertex3f(size / 2, height, -size / 2);
    glEnd();

    glPopMatrix();

    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
}



void Floor::setPosition(const cyclone::Vector3 &pos) {
    cyclone::Vector3 newPos = pos;
    newPos.y = height; // Keep floor at its fixed height
    body->setPosition(newPos);
}
