/**
 * File Name: Mover.h
 * Author: Alexandre Kévin DE FREITAS MARTINS
 * Creation Date: 3/5/2025
 * Description: This is the Mover.h
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

#ifndef MOVER_H
#define MOVER_H

#if defined(_WIN32) || defined(_WIN64)
#include "windows.h"
#endif

#include <GL/glut.h>
#include <functional>
#include <map>

#include "MySpring.h"
#include "core.h"
#include "iostream"
#include "particle.h"
#include "pfgen.h"

class Mover {
public:
    enum projectileType { BASE, WIND, LASER, FIREBALL, ARTILLERY, PISTOL };
    Mover();
    Mover(cyclone::Vector3 pos, cyclone::Vector3 vel, cyclone::Vector3 acc, float mass, float damping);
    explicit Mover(const cyclone::Vector3 &initPosition);

    ~Mover();

    void draw(int shadow) const;

    void update(float duration);
    void updateColor(float duration);

    void stop();
    void checkEdges() const;
    void setBase();
    void setWind();
    void setLaser();
    void setFireball();
    void setArtillery();
    void setPistol();
    void setConnection(Mover *other);
    void reset();
    float getSize() const { return size; }
    int getId() const { return _id; }
    void setProjectileType(projectileType type) { projectileType = type; }
    [[nodiscard]] projectileType getProjectileType() const { return projectileType; }
    static std::map<projectileType, const char *> getProjectileModeType() {
        return {{BASE, "BASE"},         {WIND, "WIND"},           {LASER, "LASER"},
                {FIREBALL, "FIREBALL"}, {ARTILLERY, "ARTILLERY"}, {PISTOL, "PISTOL"}};
    }
    std::map<projectileType, void (Mover::*)()> projectileMap = {
            {BASE, &Mover::setBase},         {WIND, &Mover::setWind},           {LASER, &Mover::setLaser},
            {FIREBALL, &Mover::setFireball}, {ARTILLERY, &Mover::setArtillery}, {PISTOL, &Mover::setPistol}};
    cyclone::Particle *m_particle;
    cyclone::MySpring *m_spring;

private:
    int _id;
    static int idProvider;
    float size = 2.0;
    projectileType projectileType = BASE;
    cyclone::Vector3 m_position = cyclone::Vector3(0, 0, 0);
    cyclone::ParticleGravity *m_gravity;
    cyclone::ParticleDrag *m_drag;
    cyclone::ParticleForceRegistry *m_forces;
    cyclone::Vector3 position = cyclone::Vector3(0, 3, 0);
    cyclone::Vector3 basePosition = cyclone::Vector3(0, 3, 0);
    cyclone::Vector3 ballColor = cyclone::Vector3(0.5, 0.5, 0.5);
};

#endif // MOVER_H
