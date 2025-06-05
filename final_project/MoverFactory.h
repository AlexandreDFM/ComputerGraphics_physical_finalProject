/**
 * File Name: MoverFactory.h
 * Author: Alexandre Kévin DE FREITAS MARTINS
 * Creation Date: 3/5/2025
 * Description: This is the MoverFactory.h
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

#ifndef MOVERFACTORY_H
#define MOVERFACTORY_H

#include <map>
#include "Mover.h"

class MoverFactory {
public:
    static MoverFactory &getInstance() {
        static MoverFactory instance;
        return instance;
    }

    Mover *createMover(const cyclone::Vector3 &position = cyclone::Vector3(0, 3, 0),
                       const cyclone::Vector3 &velocity = cyclone::Vector3(0, 0, 0),
                       const cyclone::Vector3 &acceleration = cyclone::Vector3(0, 0, 0), float mass = 1.0f,
                       float damping = 0.99f);

    const std::map<int, Mover *> &getMovers() const { return m_movers; }

    void cleanup();

private:
    MoverFactory() = default;

    MoverFactory(const MoverFactory &) = delete;
    MoverFactory &operator=(const MoverFactory &) = delete;

    std::map<int, Mover *> m_movers;
};

#endif // MOVERFACTORY_H
