#ifndef PLAYERCUBE_H
#define PLAYERCUBE_H

#include <FL/Fl.H>
#include <cyclone.h>

class PlayerCube {
public:
    PlayerCube();
    ~PlayerCube();

    // Movement control
    void setMovement(bool forward, bool backward, bool left, bool right);
    void update(float duration);
    void draw();

    // Getters
    cyclone::RigidBody *getBody() const { return body; }
    cyclone::Vector3 getPosition() const { return body->getPosition(); }

    // Setters
    void setPosition(const cyclone::Vector3 &pos);
    void setMoveSpeed(float speed) { moveSpeed = speed; }
    void setColor(float r, float g, float b);

private:
    cyclone::RigidBody *body;
    float moveSpeed;
    bool moveForward;
    bool moveBackward;
    bool moveLeft;
    bool moveRight;
    float cubeSize; // Size of the cube for drawing
    float colorR, colorG, colorB; // Added color components
};

#endif // PLAYERCUBE_H
