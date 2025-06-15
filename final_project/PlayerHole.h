/**
 * File Name: PlayerHole.h
 * Author: Alexandre Kévin DE FREITAS MARTINS
 * Creation Date: 15/6/2025
 * Description: This is the PlayerHole.h
 * Copyright (c) 2025 Alexandre Kévin DE FREITAS MARTINS
 * Version: 1.0.0
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the 'Software'), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef PLAYERCUBE_H
#define PLAYERCUBE_H

#define M_PI 3.14159265358979323846

#include <FL/Fl.H>
#include <iostream>
#include <cyclone.h>
#include <GL/glut.h>

class PlayerHole {
    public:
        PlayerHole();
        ~PlayerHole();

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
