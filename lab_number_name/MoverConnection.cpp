/**
 * File Name: MoverConnection.cpp
 * Author: Alexandre Kévin DE FREITAS MARTINS
 * Creation Date: 3/5/2025
 * Description: This is the MoverConnection.cpp
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

#include "MoverConnection.h"

MoverConnection::MoverConnection() {
    m_forces = new cyclone::ParticleForceRegistry();
    m_gravity = new cyclone::ParticleGravity(cyclone::Vector3(0, -10, 0));
    m_drag = new cyclone::ParticleDrag(0.1, 0.01);

    Mover *moverA = new Mover(cyclone::Vector3(5, 5, 5));
    Mover *moverB = new Mover(cyclone::Vector3(-5, 5, -5));

    moverA->setConnection(moverB);
    moverB->setConnection(moverA);

    m_movers.push_back(moverA);
    m_movers.push_back(moverB);

    m_forces->add(moverA->m_particle, m_gravity);
    m_forces->add(moverB->m_particle, m_gravity);
    m_forces->add(moverA->m_particle, m_drag);
    m_forces->add(moverB->m_particle, m_drag);
    m_forces->add(moverA->m_particle, moverA->m_spring);
    m_forces->add(moverB->m_particle, moverB->m_spring);
}

MoverConnection::~MoverConnection() {
    delete m_gravity;
    delete m_forces;
    for (const auto mover: m_movers) {
        delete mover;
    }
}

void MoverConnection::update(const float duration) const {
    m_forces->updateForces(duration);
    for (const auto mover: m_movers) {
        mover->update(duration);
    }
}

void MoverConnection::updateColor(const float duration) const {
    for (const auto mover: m_movers) {
        mover->updateColor(duration);
    }
}

void MoverConnection::draw(const int shadow) const {
    for (unsigned int i = 0; i < m_movers.size(); i++) {
        m_movers[i]->draw(shadow);
    }

    glBegin(GL_LINE_STRIP);
    for (const auto m_mover: m_movers) {
        const cyclone::Vector3 p = m_mover->m_particle->getPosition();
        glVertex3f(static_cast<float>(p.x), static_cast<float>(p.y), static_cast<float>(p.z));
    }
    glEnd();
}

void MoverConnection::setInGlWindowMovers(std::map<int, Mover *> &winMovers) {
    for (const auto mover: m_movers) {
        winMovers[mover->getId()] = mover;
    }
}
