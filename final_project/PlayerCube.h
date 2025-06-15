#ifndef PLAYERCUBE_H
#define PLAYERCUBE_H

#define M_PI 3.14159265358979323846

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
        float getSwallowRadius() const { return swallowRadius; }
        cyclone::Vector3 getPosition() const { return body->getPosition(); }

        // Setters
        void setPosition(const cyclone::Vector3 &pos);
        void setMoveSpeed(float speed) { moveSpeed = speed; }
        void setColor(float r, float g, float b);

        // Physics interaction
        void checkSwallowObjects(std::vector<cyclone::RigidBody *> &objects);

    private:
        cyclone::RigidBody *body;
        float swallowRadius;
        float moveSpeed;
        bool moveForward;
        bool moveBackward;
        bool moveLeft;
        bool moveRight;
        float cubeSize; // Size of the cube for drawing
        float colorR, colorG, colorB; // Added color components
};

#endif // PLAYERCUBE_H
