/**
 * File Name: MyGlWindow.cpp
 * Author: Alexandre Kévin DE FREITAS MARTINS
 * Creation Date: 3/5/2025
 * Description: This is the MyGlWindow.cpp
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

#include "MyGlWindow.h"
#include "MoverFactory.h"

#include <chrono>
#include <iostream>

#include <FL/Fl_Button.H>

#include "DrawUtils.h"
#include "timing.h"

static double DEFAULT_VIEW_POINT[3] = {30, 30, 30};
static double DEFAULT_VIEW_CENTER[3] = {0, 0, 0};
static double DEFAULT_UP_VECTOR[3] = {0, 1, 0};

MyGlWindow::MyGlWindow(int x, int y, int w, int h) : Fl_Gl_Window(x, y, w, h) {

    mode(FL_RGB | FL_ALPHA | FL_DOUBLE | FL_STENCIL);

    fieldOfView = 45;

    glm::vec3 viewPoint(DEFAULT_VIEW_POINT[0], DEFAULT_VIEW_POINT[1], DEFAULT_VIEW_POINT[2]);
    glm::vec3 viewCenter(DEFAULT_VIEW_CENTER[0], DEFAULT_VIEW_CENTER[1], DEFAULT_VIEW_CENTER[2]);
    glm::vec3 upVector(DEFAULT_UP_VECTOR[0], DEFAULT_UP_VECTOR[1], DEFAULT_UP_VECTOR[2]);

    float aspect = (static_cast<float>(w) / static_cast<float>(h));
    m_viewer = new Viewer(viewPoint, viewCenter, upVector, 45.0f, aspect);

    // Use the factory to create Movers
    MoverFactory &factory = MoverFactory::getInstance();
    Mover *first_object = factory.createMover(cyclone::Vector3(0, 2, 0));
    m_movers = factory.getMovers();

    TimingData::init();
    run = 0;
    selected = -1;
}

void MyGlWindow::setupLight(float x, float y, float z) {
    // set up the lighting
    GLfloat lightPosition[] = {500.0f, 900.0f, 500.0f, 1.0f};
    GLfloat lightPosition2[] = {1.0f, 0.0f, 0.0f, 0.0f};
    GLfloat lightPosition3[] = {0.0f, -1.0f, 0.0f, 0.0f};

    GLfloat violetLight[] = {0.5f, 0.1f, 0.5f, 1.0f};
    GLfloat whiteLight[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat whiteLight2[] = {0.3f, 0.3f, 0.3f, 1.0f};
    GLfloat blueLight[] = {0.1f, 0.1f, 0.3f, 1.0f};

    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_LIGHT2);
    glEnable(GL_DEPTH);

    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    lightPosition[0] = x;
    lightPosition[1] = y;
    lightPosition[2] = z;

    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, whiteLight);

    glLightfv(GL_LIGHT1, GL_POSITION, lightPosition2);
    glLightfv(GL_LIGHT1, GL_AMBIENT, whiteLight2);

    glLightfv(GL_LIGHT2, GL_POSITION, lightPosition3);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, whiteLight);


    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void setupObjects() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, 0x0, 0x0);
    glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
    glStencilMask(0x1); // only deal with the 1st bit
}

void MyGlWindow::drawStuff() {
    glColor4f(1, 1, 0, 0.5);
    polygonf(4, 20., 0., -25., 20., 0., 25., -20., 30., 25., -20., 30., -25.);
}

void MyGlWindow::draw() {

    glViewport(0, 0, w(), h());

    // clear the window, be sure to clear the Z-Buffer too
    glClearColor(0.2f, 0.2f, .2f, 0); // background should be blue


    glClearStencil(0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glEnable(GL_DEPTH);

    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    // now draw the ground plane
    setProjection();
    setupFloor();

    glPushMatrix();
    drawFloor(200, 20);
    glPopMatrix();

    setupLight(m_viewer->getViewPoint().x, m_viewer->getViewPoint().y, m_viewer->getViewPoint().z);

    // Add a sphere to the scene.
    // Draw axises
    glLineWidth(3.0f);
    glBegin(GL_LINES);
    glColor3f(1, 0, 0);

    glVertex3f(0.0f, 0.1f, 0.0f);
    glVertex3f(0.0f, 100.0f, 0.0f);

    glColor3f(0.0f, 1.0f, 0.0f);

    glVertex3f(0.0f, 0.1f, 0.0f);
    glVertex3f(100.0f, 0.1f, 0.0f);

    glColor3f(0.0f, 0.0f, 1.0f);

    glVertex3f(0.0f, 0.1f, 0.0f);
    glVertex3f(0.0f, 0.1f, 100.0f);
    glEnd();
    glLineWidth(1.0f);

    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    // Draw shadow
    setupShadows();
    if (!m_movers.empty()) {
        for (auto mover: m_movers) {
            mover.second->draw(1);
        }
    }
    if (!m_moverConnection.empty()) {
        for (auto &mover: m_moverConnection) {
            mover->draw(1);
        }
    }
    unsetupShadows();

    glDisable(GL_BLEND);

    // Draw objects
    glPushMatrix();
    if (!m_movers.empty()) {
        for (auto mover: m_movers) {
            mover.second->draw(0);
        }
    }
    if (!m_moverConnection.empty()) {
        for (auto &mover: m_moverConnection) {
            mover->draw(0);
        }
    }
    glPopMatrix();

    putText("STUDENT_ID_AND_NAME", 10, 10, 0.5, 0.5, 1);
    putText(getProjectileMode(), 10, 50, 0.5, 0.5, 1);

    glViewport(0, 0, w(), h());
    setProjection();
    glEnable(GL_COLOR_MATERIAL);
}

void MyGlWindow::resetTest() {
    if (!m_movers.empty()) {
        for (auto mover: m_movers) {
            mover.second->reset();
            (mover.second->*(mover.second->projectileMap[mover.second->getProjectileType()]))();
        }
    }
}

void MyGlWindow::update() {
    TimingData::update();

    if (!run)
        return;

    const float duration = static_cast<float>(TimingData::get().lastFrameDuration) * 0.003f;

    if (!m_moverConnection.empty()) {
        for (auto &mover: m_moverConnection) {
            mover->updateColor(static_cast<float>(TimingData::get().lastFrameTimestamp));
        }
    }

    if (!m_movers.empty())
        for (const auto mover: m_movers)
            mover.second->updateColor(static_cast<float>(TimingData::get().lastFrameTimestamp));

    if (duration <= 0.0f)
        return;

    if (!m_movers.empty())
        for (const auto mover: m_movers)
            mover.second->update(duration);

    if (!m_moverConnection.empty()) {
        for (auto &mover: m_moverConnection) {
            mover->update(duration);
        }
    }
}

void MyGlWindow::doPick() {
    make_current(); // since we'll need to do some GL stuff

    const int mx = Fl::event_x(); // where is the mouse?
    const int my = Fl::event_y();

    // get the viewport - most reliable way to turn mouse coords into GL coords
    int viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    // set up the pick matrix on the stack - remember, FlTk is
    // upside down!
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPickMatrix((double) mx, (double) (viewport[3] - my), 5, 5, viewport);

    // now set up the projection
    setProjection(false);

    // now draw the objects - but really only see what we hit
    GLuint buf[100];
    glSelectBuffer(100, buf);
    glRenderMode(GL_SELECT);
    glInitNames();
    glPushName(0);

    if (!m_movers.empty()) {
        for (auto mover: m_movers) {
            mover.second->draw(0);
        }
    }

    if (!m_moverConnection.empty()) {
        for (auto &mover: m_moverConnection) {
            mover->draw(0);
        }
    }

    // Draw the cubes, loading the names as we go
    // for (size_t i = 0; i < world->points.size(); ++i) {
    //     glLoadName((GLuint)(i + 1));
    //	   draw();
    // }


    // Go back to drawing mode, and see how picking did
    if (int hits = glRenderMode(GL_RENDER)) {
        // warning; this just grabs the first object hit - if there
        // are multiple objects, you really want to pick the closest
        // one - see the OpenGL manual
        // remember: we load names that are one more than the index
        // selected = buf[3] - 1;
        selected = buf[3];
    } else {
        // nothing hit, nothing selected
        selected = -1;
    }
    // printf("Selected Cube %d\n", selectedCube);
}


void MyGlWindow::setProjection(int clearProjection)
//==========================================================================
{
    glMatrixMode(GL_PROJECTION);
    glViewport(0, 0, w(), h());
    if (clearProjection)
        glLoadIdentity();
    // compute the aspect ratio so we don't distort things
    const double aspect = static_cast<double>(w()) / static_cast<double>(h());
    gluPerspective(fieldOfView, aspect, 1, 1000);

    // put the camera where we want it to be
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    // use the transformation in the ArcBall

    gluLookAt(m_viewer->getViewPoint().x, m_viewer->getViewPoint().y, m_viewer->getViewPoint().z,
              m_viewer->getViewCenter().x, m_viewer->getViewCenter().y, m_viewer->getViewCenter().z,
              m_viewer->getUpVector().x, m_viewer->getUpVector().y, m_viewer->getUpVector().z);

    //	glDisable(GL_BLEND);
}

static int last_push;
int m_pressedMouseButton;
int m_lastMouseX;
int m_lastMouseY;

int MyGlWindow::handle(int e)
//==========================================================================
{
    static cyclone::Vector3 p1, p2;
    static double t1, t2;
    int key;

    switch (e) {
        case FL_SHOW:
            // you must handle this, or not be seen!
            show();
            return 1;
        case FL_FOCUS:
        case FL_UNFOCUS:
            return 1;
        case FL_PUSH:
            m_pressedMouseButton = Fl::event_button();
            m_lastMouseX = Fl::event_x();
            m_lastMouseY = Fl::event_y();

            if (Fl::event_button() == FL_MIDDLE_MOUSE) {
                make_current();
                return 1;
            }

            if (m_pressedMouseButton == 1) {
                doPick();
                if (selected >= 0)
                    std::cout << "picked is " << selected << std::endl;
                damage(1);
                return 1;
            };
            break;
        case FL_RELEASE:
            if (selected >= 0 && m_pressedMouseButton == 1) {
                t2 = TimingData::get().lastFrameTimestamp;
                p2 = m_movers[selected]->m_particle->getPosition();
                cyclone::Vector3 newVelocity = p2 - p1;
                m_movers[selected]->m_particle->setVelocity(newVelocity);

                // Check collision with virtual walls
                cyclone::Vector3 pos = m_movers[selected]->m_particle->getPosition();
                float size = m_movers[selected]->getSize();
                if (pos.x < -100 + size || pos.x > 100 - size) {
                    newVelocity.x = -newVelocity.x;
                }
                if (pos.z < -100 + size || pos.z > 100 - size) {
                    newVelocity.z = -newVelocity.z;
                }
                m_movers[selected]->m_particle->setVelocity(newVelocity);

                run = 1; // Enable the simulation
                ui->value(1); // Turn on the run button
                selected = -1;
                damage(1);
                return 1;
            }
            m_pressedMouseButton = 0;
            break;
        case FL_DRAG: // if the user drags the mouse
            // std::cout << "Is Pressed : " << m_pressedMouseButton << std::endl;
            if (selected >= 0 && m_pressedMouseButton == 1) {

                double r1x, r1y, r1z, r2x, r2y, r2z;
                getMouseLine(r1x, r1y, r1z, r2x, r2y, r2z);

                double rx, ry, rz;

                mousePoleGo(r1x, r1y, r1z, r2x, r2y, r2z,
                            static_cast<double>(m_movers[selected]->m_particle->getPosition().x),
                            static_cast<double>(m_movers[selected]->m_particle->getPosition().y),
                            static_cast<double>(m_movers[selected]->m_particle->getPosition().z), rx, ry, rz,
                            (Fl::event_state() & FL_CTRL) != 0);

                const cyclone::Vector3 v(rx, ry, rz);

                m_movers[selected]->m_particle->setPosition(v);

                damage(1);
            } else {
                const double fractionChangeX =
                        static_cast<double>(Fl::event_x() - m_lastMouseX) / static_cast<double>(this->w());
                const double fractionChangeY =
                        static_cast<double>(m_lastMouseY - Fl::event_y()) / static_cast<double>(this->h());

                // ALT+Click for camera movement
                if (Fl::event_state(FL_ALT)) {
                    if (m_pressedMouseButton == FL_LEFT_MOUSE) {
                        // Move the camera position
                        m_viewer->translate(-static_cast<float>(fractionChangeX), -static_cast<float>(fractionChangeY),
                                            true);
                    }
                } else if (Fl::event_button2()) {
                    // Move the camera position
                    m_viewer->translate(-static_cast<float>(fractionChangeX), -static_cast<float>(fractionChangeY),
                                        true);
                } else { // Normal Click for camera movement
                    if (m_pressedMouseButton == FL_LEFT_MOUSE) {
                        // Adjust the camera orientation
                        m_viewer->rotate(static_cast<float>(fractionChangeX), static_cast<float>(fractionChangeY));
                    }
                }

                m_lastMouseX = Fl::event_x();
                m_lastMouseY = Fl::event_y();
                redraw();
            }
            return 1;
        case FL_MOUSEWHEEL: // Handle the scroll event for zoom
            if (Fl::event_dy() < 0) { // Scroll up
                m_viewer->zoom(0.1f); // Adjust the zoom increment if needed
            } else if (Fl::event_dy() > 0) { // Scroll down
                m_viewer->zoom(-0.1f); // Adjust the zoom decrement if needed
            }
            redraw();
            return 1;
        case FL_KEYBOARD:
            key = Fl::event_key();
            if (key == FL_Up) { // Use the Up arrow key for unzoom
                m_viewer->zoom(-0.1f); // Adjust the zoom factor as needed
                redraw();
                return 1;
            }
            if (key == FL_Down) { // Use the Down arrow key for zoom
                m_viewer->zoom(0.1f); // Adjust the zoom factor as needed
                redraw();
                return 1;
            }
            return 0;
        default:
            return Fl_Gl_Window::handle(e);
    }

    return 0;
}

/* get the mouse in NDC */
void MyGlWindow::getMouseNDC(float &x, float &y) {
    // notice, we put everything into doubles so we can do the math
    const float mx = static_cast<float>(Fl::event_x()); // remeber where the mouse went down
    float my = static_cast<float>(Fl::event_y());

    // we will assume that the viewport is the same as the window size
    const float wd = static_cast<float>(w());
    const float hd = static_cast<float>(h());

    // remember that FlTk has Y going the wrong way!
    my = hd - my;

    x = (mx / wd) * 2.0f - 1.f;
    y = (my / hd) * 2.0f - 1.f;
}

void drawStrokeText(const char *string, int x, int y, int z) {
    if (string == nullptr)
        return;
    glPushMatrix();
    glTranslated(x, y + 8, z);
    glScaled(0.2, 0.2, 0.2);

    for (const char *c = string; *c != '\0'; c++) {
        glutStrokeCharacter(GLUT_STROKE_ROMAN, *c);
    }
    glPopMatrix();
}

void MyGlWindow::putText(const char *str, int x, int y, float r, float g, float b) {
    glDisable(GL_LIGHTING);

    glMatrixMode(GL_PROJECTION);

    glLoadIdentity();

    glMatrixMode(GL_MODELVIEW);

    glLoadIdentity();

    ortho();

    glDisable(GL_LIGHTING);

    glDisable(GL_DEPTH_TEST);

    glColor3f(r, g, b);

    drawStrokeText(str, x, y, 0);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}

void MyGlWindow::setProjectileMode() const {
    // if (m_mover) {
    // 	// Logic to choose the mode of the projectile
    // 	// For example, cycle through the projectile types
    // 	int currentType = m_mover->getProjectileType();
    // 	int nextType = (currentType + 1) % 5; // Assuming 5 projectile types
    // 	m_mover->setProjectileType(static_cast<enum Mover::projectileType>(nextType));
    // 	(m_mover->*(m_mover->projectileMap[m_mover->getProjectileType()]))();
    // }
    if (!m_movers.empty()) {
        for (auto mover: m_movers) {
            // Logic to choose the mode of the projectile
            // For example, cycle through the projectile types
            int currentType = mover.second->getProjectileType();
            int nextType = (currentType + 1) % 5; // Assuming 5 projectile types
            mover.second->setProjectileType(static_cast<enum Mover::projectileType>(nextType));
            (mover.second->*(mover.second->projectileMap[mover.second->getProjectileType()]))();
        }
    }
}

const char *MyGlWindow::getProjectileMode() const {
    if (!m_movers.empty()) {
        return Mover::getProjectileModeType().at(m_movers.begin()->second->getProjectileType());
    }
    return nullptr;
}

void MyGlWindow::step() {
    TimingData::update();

    float duration = 0.03f; // or 0.06

    for (auto mover: m_movers) {
        mover.second->update(duration);
    }

    std::cout << "step" << std::endl;
}
