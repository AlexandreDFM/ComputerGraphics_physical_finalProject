/**
 * File Name: MoverFactory.cpp
 * Author: Alexandre Kévin DE FREITAS MARTINS
 * Creation Date: 3/5/2025
 * Description: This is the MoverFactory.cpp
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

#include "MoverFactory.h"

Mover *MoverFactory::createMover(const cyclone::Vector3 &position, const cyclone::Vector3 &velocity,
                                 const cyclone::Vector3 &acceleration, float mass, float damping) {
    Mover *mover = new Mover(position, velocity, acceleration, mass, damping);
    m_movers[mover->getId()] = mover; // Store the mover by its ID
    return mover;
}

MoverConnection *MoverFactory::createMoverConnection() {
    MoverConnection *connection = new MoverConnection();
    m_connections.push_back(connection);

    // Add Movers from the connection to the factory's map
    connection->setInGlWindowMovers(m_movers);
    return connection;
}

void MoverFactory::cleanup() {
    // Delete all Movers
    for (auto &pair: m_movers) {
        delete pair.second;
    }
    m_movers.clear();

    // Delete all MoverConnections
    for (auto connection: m_connections) {
        delete connection;
    }
    m_connections.clear();
}
