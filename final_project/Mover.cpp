/**
 * File Name: Mover.cpp
 * Author: Alexandre Kévin DE FREITAS MARTINS
 * Creation Date: 3/5/2025
 * Description: This is the Mover.cpp
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

#include "Mover.h"

int Mover::idProvider = 0;

Mover::Mover() : _id(++idProvider) {
    position = cyclone::Vector3(0, 3, 0);
    basePosition = cyclone::Vector3(0, 3, 0);
    size = 2.0;
    m_particle = new cyclone::Particle();

    m_gravity = new cyclone::ParticleGravity(cyclone::Vector3(0, -10, 0));
    m_drag = new cyclone::ParticleDrag(0.1, 0.01);
    m_forces = new cyclone::ParticleForceRegistry(); // Container
    m_forces->add(m_particle, m_gravity);
    m_forces->add(m_particle, m_drag);

    setProjectileType(BASE);
    setBase();
}

Mover::Mover(const cyclone::Vector3 &initPosition) :
    _id(++idProvider), position(initPosition), basePosition(initPosition) {
    size = 2.0;

    // Initialize the particle
    m_particle = new cyclone::Particle();
    m_particle->setPosition(initPosition);
    m_particle->setVelocity(0, 0, 0);
    m_particle->setMass(10.0f);
    m_particle->setDamping(0.9f);
    m_particle->setAcceleration(cyclone::Vector3(0, 0, 0));

    // Initialize the force registry
    m_forces = new cyclone::ParticleForceRegistry();

    // Add gravity force
    m_gravity = new cyclone::ParticleGravity(cyclone::Vector3(0, -10, 0));
    m_forces->add(m_particle, m_gravity);

    // Add drag force
    m_drag = new cyclone::ParticleDrag(0.1, 0.01);
    m_forces->add(m_particle, m_drag);

    // Initialize the spring (set later via setConnection)
    m_spring = nullptr;

    setProjectileType(BASE);
}

Mover::Mover(cyclone::Vector3 pos, cyclone::Vector3 vel, cyclone::Vector3 acc, float mass, float damping) :
    _id(++idProvider) {
    position = pos;
    basePosition = pos;
    size = 2.0;
    m_particle = new cyclone::Particle();

    m_gravity = new cyclone::ParticleGravity(cyclone::Vector3(0, -10, 0));
    m_drag = new cyclone::ParticleDrag(0.1, 0.1);
    m_forces = new cyclone::ParticleForceRegistry(); // Container
    m_forces->add(m_particle, m_gravity);
    m_forces->add(m_particle, m_drag);

    m_particle->setPosition(pos);
    m_particle->setVelocity(vel);
    m_particle->setAcceleration(acc);
    m_particle->setMass(mass);
    m_particle->setDamping(damping);
}

Mover::~Mover() {
    delete m_particle;
    delete m_gravity;
    delete m_drag;
    delete m_forces;
    delete m_spring;
}

void Mover::draw(const int shadow) const {
    cyclone::Vector3 position;
    m_particle->getPosition(&position); // get the current pos of particle

    if (shadow) {
        glColor4f(0.1f, 0.1f, 0.1f, 0.5f);
    } else {
        glLoadName(_id);
        glColor3f(static_cast<float>(ballColor.x), static_cast<float>(ballColor.y), static_cast<float>(ballColor.z));
    }

    glPushMatrix();

    glTranslatef(static_cast<float>(position.x), static_cast<float>(position.y), static_cast<float>(position.z));
    glutSolidSphere(size, 30, 30); // size = 2.0

    glPopMatrix();
}

void Mover::checkEdges() const {
    cyclone::Vector3 position;
    cyclone::Vector3 velocity;
    m_particle->getPosition(&position);
    m_particle->getVelocity(&velocity);

    if (position.x > 100) {
        position.x = 100;
        velocity.x = -velocity.x; // reverse the x velocity
        m_particle->setPosition(position);
        m_particle->setVelocity(velocity);
    }
    if (position.x < -100) {
        position.x = -100;
        velocity.x = -velocity.x; // reverse the x velocity
        m_particle->setPosition(position);
        m_particle->setVelocity(velocity);
    }
    if (position.y > 100) {
        position.y = 100;
        velocity.y = -velocity.y; // reverse the y velocity
        m_particle->setPosition(position);
        m_particle->setVelocity(velocity);
    }
    if (position.y < size) {
        position.y = size;
        velocity.y = -velocity.y; // reverse the y velocity
        m_particle->setPosition(position);
        m_particle->setVelocity(velocity);
    }
    if (position.z > 100) {
        position.z = 100;
        velocity.z = -velocity.z; // reverse the z velocity
        m_particle->setPosition(position);
        m_particle->setVelocity(velocity);
    }
    if (position.z < -100) {
        position.z = -100;
        velocity.z = -velocity.z; // reverse the z velocity
        m_particle->setPosition(position);
        m_particle->setVelocity(velocity);
    }
}

void Mover::update(float duration) {
    m_forces->updateForces(duration);
    m_particle->integrate(duration);
    checkEdges();
}

void Mover::updateColor(float duration) {
    float time = duration * 0.001f;
    float r = (sin(time * 2.0f) + 1.0f) / 2.0f;
    float g = (sin(time * 2.0f + 2.0f) + 1.0f) / 2.0f;
    float b = (sin(time * 2.0f + 4.0f) + 1.0f) / 2.0f;
    ballColor = cyclone::Vector3(r, g, b);
}

void Mover::stop() {
    position = cyclone::Vector3(0, 3, 0);
    size = 2.0;
}

void Mover::setBase() {
    m_particle->setMass(1.0f);
    m_particle->setVelocity(0, 0, 0);
    m_particle->setAcceleration(cyclone::Vector3::GRAVITY);
    m_particle->setDamping(0.99f);
}

void Mover::setWind() {
    cyclone::Vector3 wind(1.0f, 0, 0);

    m_particle->addForce(wind);
    m_particle->setPosition(basePosition); // reset pos to initial pos
    m_particle->setVelocity(0, 0, 0); // initial vel
    m_particle->setMass(1.0f); // mass
    m_particle->setDamping(0.99f); // damping
    m_particle->setAcceleration(cyclone::Vector3::GRAVITY); // initial acc.
}

void Mover::setLaser() {
    m_particle->setMass(0.1f);
    m_particle->setVelocity(0, 0, 100);
    m_particle->setAcceleration(0, 0, 0);
    m_particle->setDamping(0.99f);
}

void Mover::setFireball() {
    m_particle->setMass(1.0f);
    m_particle->setVelocity(0.0f, 0.0f, 10.0f);
    m_particle->setAcceleration(0.0f, 0.6f, 0.0f);
    m_particle->setDamping(0.9f);
}

void Mover::setArtillery() {
    m_particle->setMass(200.0f);
    m_particle->setVelocity(0.0f, 30.0f, 40.0f);
    m_particle->setAcceleration(0.0f, -20.0f, 0.0f);
    m_particle->setDamping(0.99f);
}

void Mover::setPistol() {
    m_particle->setMass(2.0f);
    m_particle->setVelocity(0.0f, 0.0f, 35.0f);
    m_particle->setAcceleration(0.0f, -1.0f, 0.0f);
    m_particle->setDamping(0.99f);
}

void Mover::reset() {
    m_particle->setPosition(0, 3, 0); // initial pos
    m_particle->setVelocity(0, 0, 0); // initial vel
    m_particle->setMass(1.0f); // mass
    m_particle->setDamping(0.99f); // damping
    m_particle->setAcceleration(cyclone::Vector3::GRAVITY); // initial acc.
}

void Mover::setConnection(Mover *other) { m_spring = new cyclone::MySpring(other->m_particle, 20.0f, 3.0f); }
