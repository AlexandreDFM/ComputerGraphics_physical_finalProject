/**
 * File Name: main.cpp
 * Author: Alexandre Kévin DE FREITAS MARTINS
 * Creation Date: 3/5/2025
 * Description: The main routine makes the window, and then runs an even loop
 *              until the window is closed
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

long lastRedraw;
int frameRate = 60;

Fl_Group *widgets;
Fl_Light_Button *run_btn;

void changeFrameCB(Fl_Widget *w, void *data) {
    const Fl_Choice *widget = static_cast<Fl_Choice *>(w);
    int i = widget->value();
    const char *menu = widget->text(i);
    frameRate = atoi(menu);

    MyGlWindow *win = static_cast<MyGlWindow *>(data);
    win->redraw();
    win->take_focus();
}

void idleCB(void *w) {
    MyGlWindow *win = static_cast<MyGlWindow *>(w);
    if (clock() - lastRedraw > CLOCKS_PER_SEC / frameRate) {
        lastRedraw = clock();
        win->update();
    }
    win->redraw();
    win->take_focus();
}

void runGame(Fl_Widget *o, void *data) {
    const Fl_Button *b = static_cast<Fl_Button *>(o);
    MyGlWindow *win = static_cast<MyGlWindow *>(data);
    if (b->value())
        win->run = 1;
    else
        win->run = 0;
    win->damage(1);
    win->take_focus();
}

void resetGame(Fl_Widget *o, void *data) {
    MyGlWindow *win = static_cast<MyGlWindow *>(data);
    win->reset();
    win->damage(1);
    win->take_focus();
}

void toggleHitboxCB(Fl_Widget *o, void *data) {
    MyGlWindow *win = static_cast<MyGlWindow *>(data);
    win->toggleHitboxes();
    win->damage(1);
    win->take_focus();
}

int main() {
    // plastic
    Fl::scheme("plastic");
    constexpr int width = 1600;
    constexpr int height = 900;
    Fl_Double_Window *wind = new Fl_Double_Window(100, 100, width, height, "GL 3D FrameWork");

    // put widgets inside of the window
    wind->begin();

    widgets = new Fl_Group(0, 0, 1600, 900);
    widgets->begin();

    MyGlWindow *gl = new MyGlWindow(10, 10, width - 20, height - 50);
    Fl::add_idle((void (*)(void *)) idleCB, gl); //

    widgets->end();
    Fl_Group::current()->resizable(widgets);

    Fl_Choice *choice = new Fl_Choice(90, height - 40, 50, 20, "FrameRate");
    choice->add("15");
    choice->add("30");
    choice->add("60");
    choice->add("120");
    choice->add("144");
    choice->add("165");
    choice->add("240");
    choice->value(2);
    choice->callback((Fl_Callback *) changeFrameCB, gl);

    constexpr int buttonWidth = 100;
    constexpr int buttonHeight = 20;
    constexpr int windowWidthCenter = width / 2 - buttonWidth / 2;

    run_btn = new Fl_Light_Button(windowWidthCenter , height - 40, buttonWidth, buttonHeight, "Run");
    run_btn->callback(runGame, gl);
    gl->ui = run_btn;

    Fl_Button *resetButton = new Fl_Button(width - buttonWidth - 20, height - 40, buttonWidth, buttonHeight, "Reset");
    resetButton->callback(resetGame, gl);

    Fl_Button *hitboxButton = new Fl_Button(width - buttonWidth * 2 - 40, height - 40, buttonWidth, buttonHeight, "Toggle Hitbox");
    hitboxButton->callback(toggleHitboxCB, gl);

    wind->end();

    // this actually opens the window
    wind->show();

    Fl::run();
    delete wind;

    return 1;
}
