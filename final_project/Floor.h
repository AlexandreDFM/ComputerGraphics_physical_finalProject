#ifndef FLOOR_H
#define FLOOR_H

#include <GL/glut.h>
#include <cyclone.h>

class Floor {
public:
    Floor(int size = 200, float height = 0.0f);
    ~Floor();

    // Drawing methods
    void draw(GLuint textureID);
    void setupFloor();

    // Physics properties
    cyclone::RigidBody *getBody() const { return body; }
    void setPosition(const cyclone::Vector3 &pos);
    cyclone::Vector3 getPosition() const { return body->getPosition(); }

private:
    cyclone::RigidBody *body;
    float size; // Size of the floor
    float height; // Height of the floor (y position)
};

#endif // FLOOR_H
