#pragma once
#include <FL/glut.H>
#include <GL/gl.h>
#include <vector>

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

    cyclone::RigidBody* getBody() {
        return body;
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
    
    bool isValid() const { return valid; }
    bool isSwallowed() const { return swallowed; }
    void invalidate() {
        valid = false;
        delete body;
        body = nullptr;
    }
    void setSwallowed(bool swallowed) { this->swallowed = swallowed; }

private:
    bool isBeingDragged;
    bool valid = true;
    bool swallowed = false;
};

class SimplePhysics {
public:
    static const unsigned maxContacts = 512;
    std::vector<Box*> boxData;
    cyclone::Contact* contacts;
    cyclone::CollisionData* cData;
    cyclone::ContactResolver* resolver;

    SimplePhysics() {
        contacts = new cyclone::Contact[maxContacts];
        cData = new cyclone::CollisionData();
        cData->contactArray = contacts;
        resolver = new cyclone::ContactResolver(maxContacts * 2, maxContacts * 2, 0.001f, 0.001f);
        // Initialize vector with new Box objects
        for (int i = 0; i < 100; i++) {
            boxData.push_back(new Box());
        }
        reset();
    }

    ~SimplePhysics() {
        // Clean up Box objects
        for (Box* box : boxData) {
            delete box;
        }
        boxData.clear();
        
        delete[] contacts;
        delete cData;
        delete resolver;
    }

    void reset();
    void generateContacts();
    void update(cyclone::real duration);
    void render(int shadow);

    void drawWithNames() {
        for (int i = 0; i < boxData.size(); i++) {
            boxData[i]->drawWithName(i + 1); // Use 1-based indices for picking
        }
    }

    Box *getBox(int index) {
        if (boxData.size() > index) {
            return boxData[index];
        }
        return nullptr;
    }

    std::vector<cyclone::RigidBody *> getAllRigidBoxes() {
        std::vector<cyclone::RigidBody *> boxes;
        for (auto i : boxData) {
            if (i->isValid() && i->getBody() != nullptr) {
                boxes.push_back(i->getBody());
            }
        }
        return boxes;
    }

    void removeBox(cyclone::RigidBody* body) {
        for (int i = 0; i < boxData.size(); i++) {
            if (boxData[i]->getBody() == body) {
                boxData[i]->invalidate();
                break;
            }
        }
    }

    void setSwallowed(cyclone::RigidBody* body, bool swallowed) {
        for (int i = 0; i < boxData.size(); i++) {
            if (boxData[i]->getBody() == body) {
                boxData[i]->setSwallowed(swallowed);
                break;
            }
        }
    }
};