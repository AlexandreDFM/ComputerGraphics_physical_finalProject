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

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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

    // Create game objects
    createGameObjects();

    textureLoaded = false;
}

MyGlWindow::~MyGlWindow() {
    // Clean up rigid bodies
    for (auto body: gameRigidBodies) {
        if (body != playerCube->getBody() && body != floor->getBody()) {
            delete body;
        }
    }
    delete playerCube;
    delete floor;
}

void MyGlWindow::toggleHitboxes()
{
    simplePhysics->toggleHitboxes();
}

void MyGlWindow::createGameObjects() {
    // Create score object
    score = new Score(0);

    // Create the floor
    floor = new Floor();
    gameRigidBodies.push_back(floor->getBody());

    outFloor = new Floor(350, -0.1f);

    // Create the player cube
    playerCube = new PlayerHole();
    gameRigidBodies.push_back(playerCube->getBody());

    // Create the simple physics world with boxes
    simplePhysics = new SimplePhysics();

    playerCube->setSimplePhysics(simplePhysics);
    playerCube->setScore(score);
    //simplePhysics->update(0.3f);

    // Integrate model into the physics system
    AddModelToRigidBodies(*simplePhysics);
}

void MyGlWindow::AddModelToRigidBodies(SimplePhysics& physics) {

    // const std::string apartmentPath = "../../../Models/apartment.obj";
    // const std::string treePath = "../../../Models/tree3.obj";
    const std::string apartmentPath = (std::filesystem::current_path() / "Models" / "apartment.obj").string();
    const std::string treePath = (std::filesystem::current_path() / "Models" / "tree3.obj").string();
    srand(static_cast<unsigned int>(std::time(nullptr))); // Seed the random number generator

    // Load the model
    LoadModel(apartmentPath, aptMesh);
    LoadModel(treePath, treeMesh);

    // Iterate through all rigid bodies in the physics system
    for (auto& box : physics.getBoxes()) {
        // Set the mesh for the rigid body
        box->setMesh((rand() % 2 == 0) ? aptMesh : treeMesh);
    }

    std::cout << "Model added to all rigid bodies in the physics system." << std::endl;
}

void MyGlWindow::LoadModel(std::string filename, Mesh &newMesh) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string err, warn;

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename.c_str(), nullptr, true);

    if (!warn.empty())
        std::cerr << "TinyObjLoader warning: " << warn << std::endl;
    if (!err.empty())
        std::cerr << "TinyObjLoader error: " << err << std::endl;
    if (!ret) {
        std::cerr << "Failed to load model: " << filename << std::endl;
        return;
    }

    // Compute bounding box
    float minX = FLT_MAX, minY = FLT_MAX, minZ = FLT_MAX;
    float maxX = -FLT_MAX, maxY = -FLT_MAX, maxZ = -FLT_MAX;

    float extentX = maxX - minX;
    float extentY = maxY - minY;
    float extentZ = maxZ - minZ;
    float maxExtent = std::max({extentX, extentY, extentZ});
    float targetSize = 1.0f; // You can change this to any size you want
    float scale = targetSize / maxExtent;

    for (size_t i = 0; i < attrib.vertices.size(); i += 3) {
        float x = attrib.vertices[i + 0];
        float y = attrib.vertices[i + 1];
        float z = attrib.vertices[i + 2];

        minX = std::min(minX, x);
        maxX = std::max(maxX, x);
        minY = std::min(minY, y);
        maxY = std::max(maxY, y);
        minZ = std::min(minZ, z);
        maxZ = std::max(maxZ, z);
    }

    newMesh.SetBoundingBox(cyclone::Vector3(minX, minY, minZ), cyclone::Vector3(maxX, maxY, maxZ));

    // Center of the bounding box
    float centerX = (minX + maxX) * 0.5f;
    float centerY = (minY + maxY) * 0.5f;
    float centerZ = (minZ + maxZ) * 0.5f;

    // Clear any previous data
    newMesh.clear();

    for (const auto &shape: shapes) {
        for (const auto &index: shape.mesh.indices) {
            // Vertex
            float x = attrib.vertices[3 * index.vertex_index + 0] - centerX;
            float y = attrib.vertices[3 * index.vertex_index + 1] - centerY;
            float z = attrib.vertices[3 * index.vertex_index + 2] - centerZ;
            newMesh.addVertex(x, y, z);

            // Normal
            if (!attrib.normals.empty() && index.normal_index >= 0) {
                newMesh.addNormal(attrib.normals[3 * index.normal_index + 0], attrib.normals[3 * index.normal_index + 1],
                               attrib.normals[3 * index.normal_index + 2]);
            }

            // Texture coordinate
            if (!attrib.texcoords.empty() && index.texcoord_index >= 0) {
                newMesh.addTextureCoord(attrib.texcoords[2 * index.texcoord_index + 0],
                                     1.0f - attrib.texcoords[2 * index.texcoord_index + 1]);
            }

            newMesh.addIndex(static_cast<unsigned int>(newMesh.getIndices().size()));
        }
    }
}


void MyGlWindow::LoadTexture(std::string filename, GLuint &newTextureID) {
    glGenTextures(1, &newTextureID);
    glBindTexture(GL_TEXTURE_2D, newTextureID);
    // Load the image data (you can use a library like SOIL or stb_image.h)
    int width, height, channels;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &channels, 0);
    if (data) {
        GLenum format = (channels == 3) ? GL_RGB : GL_RGBA;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
    } else {
        std::cerr << "Failed to load texture: " << filename << std::endl;
    }
    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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

void MyGlWindow::drawModel(const Mesh &modelMesh) {
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
    glColor3f(1.0f, 1.0f, 1.0f); // Set color to white for texture mapping
    glBindTexture(GL_TEXTURE_2D, textureID); // <-- bind your texture

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, modelMesh.getVertices().data());

    if (!modelMesh.getNormals().empty()) {
        glEnableClientState(GL_NORMAL_ARRAY);
        glNormalPointer(GL_FLOAT, 0, modelMesh.getNormals().data());
    }

    if (!modelMesh.getTextureCoords().empty()) {
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(2, GL_FLOAT, 0, modelMesh.getTextureCoords().data()); // <-- setup UVs
    }

    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(modelMesh.getIndices().size()), GL_UNSIGNED_INT, modelMesh.getIndices().data());

    glDisableClientState(GL_VERTEX_ARRAY);
    if (!modelMesh.getNormals().empty())
        glDisableClientState(GL_NORMAL_ARRAY);
    if (!modelMesh.getTextureCoords().empty())
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);


    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING); // Disable lighting after drawing the model
}



void MyGlWindow::draw() {
    make_current(); // Ensure context is current (usually already is in FLTK draw())
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW\n";
        exit(1);
    }

    if (!textureLoaded) {
        const std::string currentPath = std::filesystem::current_path().string();
        LoadTexture(currentPath + "/Models/apartment_texture.png", textureID);
        LoadTexture(currentPath + "/Models/Grass.png", floorTextureID);
        LoadTexture(currentPath + "/Models/Concrete.png", outFloorTextureID);
        LoadTexture(currentPath + "/Models/holeTex.png", holeTextureID);
        textureLoaded = true;
    }

    glViewport(0, 0, w(), h());

    // clear the window, be sure to clear the Z-Buffer too
    glClearColor(0.2f, 0.2f, .2f, 0); // background should be blue
    glClearStencil(0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glEnable(GL_DEPTH);
    glEnable(GL_NORMALIZE);

    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    // Draw the floor
    setProjection();

    setupLight(m_viewer->getViewPoint().x, m_viewer->getViewPoint().y, m_viewer->getViewPoint().z);

    // Draw coordinate axes
    //glLineWidth(3.0f);
    //glBegin(GL_LINES);
    //glColor3f(1, 0, 0);
    //glVertex3f(0.0f, 0.1f, 0.0f);
    //glVertex3f(0.0f, 100.0f, 0.0f);
    //glColor3f(0.0f, 1.0f, 0.0f);
    //glVertex3f(0.0f, 0.1f, 0.0f);
    //glVertex3f(100.0f, 0.1f, 0.0f);
    //glColor3f(0.0f, 0.0f, 1.0f);
    //glVertex3f(0.0f, 0.1f, 0.0f);
    //glVertex3f(0.0f, 0.1f, 100.0f);
    //glEnd();
    //glLineWidth(1.0f);

    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);


    floor->draw(floorTextureID);
    outFloor->draw(outFloorTextureID);

    glEnable(GL_COLOR_MATERIAL);
    for (auto body: gameRigidBodies) {
        if (body == playerCube->getBody()) {
            playerCube->draw(holeTextureID);
        }
    }

    simplePhysics->render(0, textureID);

    // Draw timer above the score
    char timerStr[64];
    int timerY = 35; // Y position above the score
    snprintf(timerStr, sizeof(timerStr), "Time: %.2f s", timerSeconds);
    putText(timerStr, 10, timerY, 1, 1, 0.5f);

    putText("Score :", 10, 10, 0.5, 0.5, 1);
    putText(score->getScoreString().c_str(), 125, 10, 0.5, 0.5, 1);

    if (!run) {
        // print a white background
        glColor4f(0.2f, 0.2f, 0.2f, 0.5f);
        glBegin(GL_QUADS);
        glVertex2f(0, 0);
        glVertex2f(w(), 0);
        glVertex2f(w(), h());
        glVertex2f(0, h());
        glEnd();
        glColor3f(1, 1, 1);
        // print the message in the middle of the screen
        putText("Game is not running press on the run button", w() / 2 - 300, h() / 2 - 20, 1, 1, 1);
        putText("Press 'R' to reset the game", w() / 2 - 175, h() / 2 + 20, 1, 1, 1);
    }
}

void MyGlWindow::reset() {
    run = 0;
    ui->value(0);

    // Reset timer
    resetTimer();

    // Reset score
    if (score) score->setScore(0);

    // Reset playerCube and physics state
    if (playerCube) playerCube->setPosition(cyclone::Vector3(0, 2, 0));
    for (auto body: gameRigidBodies) {
        if (body != playerCube->getBody() && body != floor->getBody()) {
            delete body;
        }
    }
    gameRigidBodies.clear();
    delete simplePhysics;

    createGameObjects();

    // Optionally, reset movement flags
    moveForward = moveBackward = moveLeft = moveRight = false;

    // Redraw window
    redraw();
}

void MyGlWindow::update() {
    TimingData::update();

    // const float duration = static_cast<float>(TimingData::get().lastFrameDuration) * 0.003f;
    // Convert milliseconds to seconds with proper scaling and add maximum time step
    float duration = static_cast<float>(TimingData::get().lastFrameDuration) * 0.001f;  // Proper conversion from ms to seconds
    const float maxTimeStep = 1.0f;
    if (duration > maxTimeStep) {
        duration = maxTimeStep;
    }

    if (!run) {
        // If not running, just update the player cube
        playerCube->setMovement(moveForward, moveBackward, moveLeft, moveRight);
        playerCube->update(duration);
        return;
    }

    if (!isTimerRunning()) {
        timerSeconds += duration;
    }

    playerCube->setMovement(moveForward, moveBackward, moveLeft, moveRight);
    playerCube->update(duration);

    std::vector<cyclone::RigidBody *> boxes = simplePhysics->getRigidBoxes();
    playerCube->checkSwallowObjects(boxes);

    simplePhysics->update(duration);

    // Force redraw to update visual position
    redraw();
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
    double orthoSize = 25;
    glOrtho(-orthoSize * aspect, orthoSize * aspect, -orthoSize, orthoSize, -1000, 1000);

    // put the camera where we want it to be
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    double distance = 50;
    double camX = playerCube->getPosition().x + distance + m_viewer->getViewPoint().x;
    double camY = playerCube->getPosition().y + distance * 0.5 + m_viewer->getViewPoint().y;
    double camZ = playerCube->getPosition().z + distance + m_viewer->getViewPoint().z;

    gluLookAt(camX, camY, camZ,
              playerCube->getPosition().x, playerCube->getPosition().y, playerCube->getPosition().z,
              m_viewer->getUpVector().x, m_viewer->getUpVector().y, m_viewer->getUpVector().z);


    //	glDisable(GL_BLEND);
}

static int last_push;
int m_pressedMouseButton;
int m_lastMouseX;
int m_lastMouseY;

int MyGlWindow::handle(int e) {
    static cyclone::Vector3 p1, p2;
    static double t1, t2;
    int key;
    bool wasPressed = false;

    switch (e) {
        case FL_SHOW:
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

                run = 1;
                ui->value(1);
                selected = -1;
                damage(1);
                return 1;
            }
            m_pressedMouseButton = 0;
            break;
        case FL_DRAG:
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

                if (!cameraLocked) {
                    if (Fl::event_state(FL_ALT)) {
                        if (m_pressedMouseButton == FL_LEFT_MOUSE) {
                            m_viewer->translate(-static_cast<float>(fractionChangeX), -static_cast<float>(fractionChangeY), true);
                        }
                    } else if (Fl::event_button2()) {
                        m_viewer->translate(-static_cast<float>(fractionChangeX), -static_cast<float>(fractionChangeY), true);
                    } else {
                        if (m_pressedMouseButton == FL_LEFT_MOUSE) {
                            m_viewer->rotate(static_cast<float>(fractionChangeX), static_cast<float>(fractionChangeY));
                        }
                    }
                }

                m_lastMouseX = Fl::event_x();
                m_lastMouseY = Fl::event_y();
                redraw();
            }
            return 1;
        case FL_MOUSEWHEEL:
            if (!cameraLocked) {
                if (Fl::event_dy() < 0)
                    m_viewer->zoom(0.1f);
                else if (Fl::event_dy() > 0)
                    m_viewer->zoom(-0.1f);
                redraw();
                return 1;
            }
            break;
        case FL_KEYDOWN:
            key = Fl::event_key();
            switch (key) {
                case 'w':
                case 'W':
                    wasPressed = true;
                    moveForward = true;
                    playerCube->setColor(1.0f, 0.0f, 0.0f);
                    playerCube->setColor(1.0f, 0.0f, 0.0f);
                    break;
                case 's':
                case 'S':
                    wasPressed = true;
                    moveBackward = true;
                    playerCube->setColor(0.0f, 1.0f, 0.0f);
                    break;
                case 'a':
                case 'A':
                    wasPressed = true;
                    moveLeft = true;
                    playerCube->setColor(0.0f, 0.0f, 1.0f);
                    break;
                case 'd':
                case 'D':
                    wasPressed = true;
                    moveRight = true;
                    playerCube->setColor(1.0f, 1.0f, 0.0f);
                    break;
                case 'r':
                    reset();
                    return 1;
                case FL_Up:
                    if (cameraLocked) break;
                    m_viewer->zoom(-0.1f);
                    redraw();
                    return 1;
                case FL_Down:
                    if (cameraLocked) break;
                    m_viewer->zoom(0.1f);
                    redraw();
                    return 1;
                default:
                    moveForward = false, moveBackward = false, moveLeft = false, moveRight = false;
                    playerCube->setColor(1.0f, 0.4f, 0.7f);
                    redraw();
                    break;
            }
            if (wasPressed) {
                playerCube->setMoveSpeed(1.0f);
                redraw();
                return 1;
            }
            return 0;
        case FL_KEYUP:
            key = Fl::event_key();
            switch (key) {
                case 'w':
                case 'W':
                    moveForward = false;
                    playerCube->setColor(1.0f, 0.4f, 0.7f);
                    break;
                case 's':
                case 'S':
                    moveBackward = false;
                    playerCube->setColor(1.0f, 0.4f, 0.7f);
                    break;
                case 'a':
                case 'A':
                    moveLeft = false;
                    playerCube->setColor(1.0f, 0.4f, 0.7f);
                    break;
                case 'd':
                case 'D':
                    moveRight = false;
                    playerCube->setColor(1.0f, 0.4f, 0.7f);
                    break;
                default:
                    break;
            }
            redraw();
            return 1;
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

// Timer control methods
void MyGlWindow::startTimer() {
    timerRunning = true;
}

void MyGlWindow::resetTimer() {
    timerSeconds = 0.0f;
    timerRunning = false;
}

bool MyGlWindow::isTimerRunning() {
    return timerRunning;
}
