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

#define NOMINMAX


#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>
#include <FL/Fl_Value_Slider.H>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Light_Button.H>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include "math.h"
#include "stdio.h"

#include <vector>
#include "core.h"

#include "Mover.h"
#include "MoverConnection.h"

#include "3DUtils.h"
#include "Vec3f.h"
#include "Viewer.h"


class MyGlWindow : public Fl_Gl_Window {
public:
    MyGlWindow(int x, int y, int w, int h);

    Fl_Light_Button *ui;
    Fl_Slider *time;

    int run;
    void update();
    void drawStuff();
    void doPick();
    void resetTest();
    int selected;
    void putText(const char *str, int x, int y, float r, float g, float b);
    void setProjectileMode() const;

    const char *getProjectileMode() const;
    void step();

private:
    void draw(); // standard FlTk
    int handle(int); // standard FlTk

    Viewer *m_viewer;
    float fieldOfView;
    std::map<int, Mover *> m_movers;
    std::vector<MoverConnection *> m_moverConnection;

    void setProjection(int clearProjection = 1);
    void getMouseNDC(float &x, float &y);
    void setupLight(float x, float y, float z);
};
