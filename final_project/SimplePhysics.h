#pragma once
#include <FL/glut.H>
#include <GL/gl.h>

#include "collide_fine.h"
#include "contacts.h"
#include "world.h"

class Box : public cyclone::CollisionBox {
public:
    Box() {
        body = new cyclone::RigidBody();
        body->setCanSleep(true);
        body->setAwake(true);
        isBeingDragged = false;
    }
    ~Box() {
        delete body;
    }

    void setState(const cyclone::Vector3& position,
                 const cyclone::Quaternion& orientation,
                 const cyclone::Vector3& extents,
                 const cyclone::Vector3& velocity) {
        body->setPosition(position);
        body->setOrientation(orientation);
        body->setVelocity(velocity);
        body->setRotation(cyclone::Vector3(0, 0, 0));
        body->setMass(1.0f);
        body->setLinearDamping(0.95f);
        body->setAngularDamping(0.8f);
        body->clearAccumulators();
        body->setAcceleration(cyclone::Vector3(0, -9.81f, 0));

        // Set up the collision box
        halfSize = extents;
        offset = cyclone::Matrix4();
        calculateInternals();
    }

    void startDragging() {
        isBeingDragged = true;
        body->setAwake(true);
        body->setCanSleep(false);
        body->setAcceleration(cyclone::Vector3(0, 0, 0));
        body->setVelocity(cyclone::Vector3(0, 0, 0));
        body->setRotation(cyclone::Vector3(0, 0, 0));
    }

    void stopDragging() {
        isBeingDragged = false;
        body->setCanSleep(true);
        body->setAcceleration(cyclone::Vector3(0, -9.81f, 0));
    }

    void setPosition(const cyclone::Vector3& position) {
        body->setPosition(position);
        if (isBeingDragged) {
            body->setVelocity(cyclone::Vector3(0, 0, 0));
            body->setRotation(cyclone::Vector3(0, 0, 0));
        }
        calculateInternals();
    }

    cyclone::Vector3 getPosition() const {
        return body->getPosition();
    }

    void draw(int shadow) {
        GLfloat mat[16];
        body->getGLTransform(mat);

        if (shadow) {
            glColor4f(0.2f, 0.2f, 0.2f, 0.5f);
        } else if (isBeingDragged) {
            glColor3f(1.0f, 0.5f, 0.5f); // Reddish color when being dragged
        } else if (body->getAwake()) {
            glColor3f(0.7f, 0.7f, 1.0f);
        } else {
            glColor3f(1.0f, 0.7f, 0.7f);
        }

        glPushMatrix();
        glMultMatrixf(mat);
        glScalef(halfSize.x * 2, halfSize.y * 2, halfSize.z * 2);
        glutSolidCube(1.0f);

        if (!shadow) {
            // Draw axes in the same transform (no extra rotation)
            glDisable(GL_LIGHTING);
            glLineWidth(3.0f);
            glBegin(GL_LINES);

            glColor3f(1, 0, 0);
            glVertex3f(0.0f, 0.0f, 0.0f);
            glVertex3f(0.0f, 5.0f, 0.0f);

            glColor3f(0.0f, 1.0f, 0.0f);
            glVertex3f(0.0f, 0.0f, 0.0f);
            glVertex3f(5.0f, 0.0f, 0.0f);

            glColor3f(0.0f, 0.0f, 1.0f);
            glVertex3f(0.0f, 0.0f, 0.0f);
            glVertex3f(0.0f, 0.0f, 5.0f);

            glEnd();
            glLineWidth(1.0f);
            glEnable(GL_LIGHTING);
        }

        glPopMatrix();
    }

    void drawWithName(int name) {
        GLfloat mat[16];
        body->getGLTransform(mat);

        glLoadName(name);
        glPushMatrix();
        glMultMatrixf(mat);
        glScalef(halfSize.x * 2, halfSize.y * 2, halfSize.z * 2);
        glutSolidCube(1.0f);
        glPopMatrix();
    }

private:
    bool isBeingDragged;
};

class SimplePhysics {
public:
    static const unsigned maxContacts = 256;
    cyclone::Contact* contacts;
    cyclone::CollisionData* cData;
    cyclone::ContactResolver* resolver;
    Box boxData[5];  // 5 boxes

    SimplePhysics() {
        contacts = new cyclone::Contact[maxContacts];
        cData = new cyclone::CollisionData();
        cData->contactArray = contacts;
        resolver = new cyclone::ContactResolver(maxContacts * 2, maxContacts * 2, 0.001f, 0.001f);
        reset();
    }

    ~SimplePhysics() {
        delete[] contacts;
        delete cData;
        delete resolver;
    }

    void reset();
    void generateContacts();
    void update(cyclone::real duration);
    void render(int shadow);

    void drawWithNames() {
        for (int i = 0; i < 5; i++) {
            boxData[i].drawWithName(i + 1); // Use 1-based indices for picking
        }
    }

    Box* getBox(int index) {
        if (index >= 1 && index <= 5) { // 1-based index
            return &boxData[index - 1];
        }
        return nullptr;
    }
};
