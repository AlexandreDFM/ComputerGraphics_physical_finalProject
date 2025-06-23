#pragma once
#include <FL/glut.H>
#include <GL/gl.h>
#include <vector>

#include "Mesh.h"
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
        mesh = Mesh(
            {},
            {},
            {},
            {}
        );
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

        cyclone::Matrix3 tensor;
        tensor.setBlockInertiaTensor(halfSize, 2.0f);
        body->setInertiaTensor(tensor);

        body->setLinearDamping(0.98f);
        body->setAngularDamping(0.95f);
        body->clearAccumulators();
        body->setAcceleration(cyclone::Vector3(0, -9.81f, 0));
        body->setCanSleep(true);

        body->calculateDerivedData();
        halfSize = extents;
        offset = cyclone::Matrix4();
        calculateInternals();
    }

    void setMesh(const Mesh& mesh) {
        this->mesh = mesh;
    }

    void startDragging() {
        isBeingDragged = true;
        body->setAwake(true);
        //body->setCanSleep(false);
        body->setAcceleration(cyclone::Vector3(0, 0, 0));
        body->setVelocity(cyclone::Vector3(0, 0, 0));
        body->setRotation(cyclone::Vector3(0, 0, 0));
    }

    void stopDragging() {
        isBeingDragged = false;
        //body->setCanSleep(true);
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

    cyclone::RigidBody *getBody() { return body; }

    static void drawAxe(int shadow) {
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
    }

    void drawHitbox(int name, int shadow) const {
        GLfloat mat[16];
        body->getGLTransform(mat);

        if (shadow) {
            glColor4f(0.2f, 0.2f, 0.2f, 0.5f);
        } else if (isBeingDragged) {
            glColor3f(1.0f, 0.5f, 0.5f);
        } else if (body->getAwake()) {
            glColor3f(0.7f, 0.7f, 1.0f);
        } else {
            glColor3f(1.0f, 0.7f, 0.7f);
        }

        glLoadName(name);
        glPushMatrix();
        glMultMatrixf(mat);
        glScalef(static_cast<GLfloat>(halfSize.x) * 2, static_cast<GLfloat>(halfSize.y) * 2, static_cast<GLfloat>(halfSize.z) * 2);
        glutWireCube(1.0f); // Draw a wireframe cube for the hitbox
        glPopMatrix();
    }

    void draw(int name, int shadow, const GLuint textureID) {
        GLfloat mat[16];
        body->getGLTransform(mat);
        const cyclone::Vector3 position = body->getPosition();

        if (shadow) {
            glColor4f(0.2f, 0.2f, 0.2f, 0.5f);
        } else if (isBeingDragged) {
            glColor3f(1.0f, 0.5f, 0.5f);
        } else if (body->getAwake()) {
            glColor3f(0.7f, 0.7f, 1.0f);
        } else {
            glColor3f(1.0f, 0.7f, 0.7f);
        }

        float modelExtentX = mesh.bboxMax.x - mesh.bboxMin.x;
        float modelExtentY = mesh.bboxMax.y - mesh.bboxMin.y;
        float modelExtentZ = mesh.bboxMax.z - mesh.bboxMin.z;

        float scaleX = (halfSize.x * 2) / modelExtentX;
        float scaleY = (halfSize.y * 2) / modelExtentY;
        float scaleZ = (halfSize.z * 2) / modelExtentZ;

        glLoadName(name);
        glPushMatrix();
        glMultMatrixf(mat);
        glScalef(static_cast<GLfloat>(scaleX), static_cast<GLfloat>(scaleY), static_cast<GLfloat>(scaleZ));
        mesh.drawModel(mesh, textureID);
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
    Mesh mesh;
    bool awake = true;
};

class SimplePhysics {
public:
    static const unsigned maxContacts = 5096;
    std::vector<Box*> boxData;
    cyclone::Contact* contacts;
    cyclone::CollisionData* cData;
    cyclone::ContactResolver* resolver;

    SimplePhysics() {
        contacts = new cyclone::Contact[maxContacts];
        cData = new cyclone::CollisionData();
        cData->contactArray = contacts;
        resolver = new cyclone::ContactResolver(maxContacts, maxContacts, 0.01f, 0.01f);
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
    void render(int shadow, const GLuint textureID);

    void drawWithNames(const GLuint textureID) {
        for (int i = 0; i < boxData.size(); i++) {
            boxData[i]->draw(i + 1, 0, textureID); // Use 1-based indices for picking
        }
    }

    Box *getBox(int index) {
        if (boxData.size() > index) {
            return boxData[index];
        }
        return nullptr;
    }

    std::vector<Box*> getBoxes() {
        return boxData;
    }

    std::vector<cyclone::RigidBody *> getRigidBoxes() {
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