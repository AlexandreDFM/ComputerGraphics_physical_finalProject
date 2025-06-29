/**
 * File Name: MyGlWindow.h
 * Author: Alexandre Kévin DE FREITAS MARTINS
 * Creation Date: 3/5/2025
 * Description: This is the MyGlWindow.h
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

#ifndef MYGLWINDOW_H
#define MYGLWINDOW_H

#define NOMINMAX
#define M_PI 3.14159265358979323846

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Counter.H>
#include <FL/Fl_Gl_Window.H>
#include <FL/Fl_Light_Button.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_Double_Window.H>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include "body.h"
#include "core.h"
#include "math.h"
#include "stdio.h"
#include "world.h"
#include "timing.h"
#include "precision.h"

#include <ctime>
#include <vector>
#include <chrono>
#include <iostream>
#include <cyclone.h>
#include <filesystem>

#include "Vec3f.h"
#include "Viewer.h"
#include "3DUtils.h"
#include "DrawUtils.h"

#include "Floor.h"
#include "Mesh.h"
#include "Mover.h"
#include "MoverFactory.h"
#include "PlayerHole.h"
#include "Score.h"
#include "SimplePhysics.h"

class MyGlWindow : public Fl_Gl_Window {
public:
    MyGlWindow(int x, int y, int w, int h);
    ~MyGlWindow();

    Fl_Light_Button *ui;
    Fl_Slider *time;

    int run;
    void update();
    void doPick();
    void reset();
    int selected;
    void putText(const char *str, int x, int y, float r, float g, float b);
    void createGameObjects();
    void AddModelToRigidBodies(SimplePhysics &physics);
    void toggleHitboxes();

    // Timer controls
    void startTimer();
    void resetTimer();
    bool isTimerRunning();
    float getTimerSeconds() const { return timerSeconds; }
    bool isTimerRunning() const { return timerRunning; }

    void setCameraLocked(bool locked) { cameraLocked = locked; }

private:
    void draw() override;
    int handle(int e) override;
    void drawModel(const Mesh &modelMesh);
    void LoadModel(std::string filename, Mesh &newMesh);
    void LoadTexture(std::string filename, GLuint &newTextureID);

    Viewer *m_viewer;
    float fieldOfView;
    std::map<int, Mover *> m_movers;
    std::vector<Mover *> m_moverConnection;

    Score *score;

    // Rigid bodies for game objects
    Floor *floor;
    Floor *outFloor; // Second floor for the building
    Mover *building;

    GLuint textureID;
    GLuint floorTextureID;
    GLuint outFloorTextureID;
    GLuint holeTextureID;
    Mesh aptMesh; // Mesh for the player cube
    Mesh treeMesh; // Mesh for the tree
    PlayerHole *playerCube;
    SimplePhysics *simplePhysics;
    std::vector<cyclone::RigidBody *> gameRigidBodies;

    // Movement state flags
    bool moveForward = false;
    bool moveBackward = false;
    bool moveLeft = false;
    bool moveRight = false;

    bool textureLoaded = false;

    // Timer state
    float timerSeconds = 0.0f;
    bool timerRunning = false;

    bool cameraLocked = true;

    void setProjection(int clearProjection = 1);
    void getMouseNDC(float &x, float &y);
    void setupLight(float x, float y, float z);
};

#endif // MYGLWINDOW_H
