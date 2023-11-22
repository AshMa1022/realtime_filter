#pragma once

// Defined before including GLEW to suppress deprecation messages on macOS
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif
#include <GL/glew.h>
#include <glm/glm.hpp>

#include <unordered_map>
#include <QElapsedTimer>
#include <QOpenGLWidget>
#include <QTime>
#include <QTimer>
#include "camera.h"
#include "utils/sceneparser.h"
#include "Shapes/cube.h"
#include "Shapes/sphere.h"
#include "utils/shaderloader.h"
#include "Shapes/shape.h"
#include "Shapes/cylinder.h"
#include "Shapes/cone.h"

class Realtime : public QOpenGLWidget
{
public:
    Realtime(QWidget *parent = nullptr);
    void finish();                                      // Called on program exit
    void sceneChanged();
    void settingsChanged();
    void saveViewportImage(std::string filePath);

public slots:
    void tick(QTimerEvent* event);                      // Called once per tick of m_timer

protected:
    void initializeGL() override;                       // Called once at the start of the program
    void paintGL() override;                            // Called whenever the OpenGL context changes or by an update() request
    void resizeGL(int width, int height) override;      // Called when window size changes

private:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void timerEvent(QTimerEvent *event) override;

    void bindHelper(Shape &shape,std::vector<float> &vertex, int index);

    // Tick Related Variables
    int m_timer;                                        // Stores timer which attempts to run ~60 times per second
    QElapsedTimer m_elapsedTimer;                       // Stores timer which keeps track of actual time between frames

    // Input Related Variables
    bool m_mouseDown = false;                           // Stores state of left mouse button
    glm::vec2 m_prev_mouse_pos;                         // Stores mouse position
    std::unordered_map<Qt::Key, bool> m_keyMap;         // Stores whether keys are pressed or not

    // Device Correction Variables
    int m_devicePixelRatio;

    Camera cam;
    GLuint m_shader; // Stores id of shader program



    GLuint m_shade;
    std::vector<GLuint> m_vbos;    // Stores id of VBO
    std::vector<GLuint> m_vaos;    // Stores id of VAO

    Cube cube;
    Sphere sphere;
    Cylinder cylinder;
    Cone cone;
    std::vector<float> cube_vertex;
    std::vector<float> sphere_vertex;
    std::vector<float> cyl_vertex;
    std::vector<float> cone_vertex;

    int param1;
    int param2;
    int cout;
    bool initialized = false;
    std::vector<RenderShapeData> shapes;
    std::vector<SceneLightData> lights;
    SceneGlobalData globalData;

    glm::mat4 m_model;
    glm::mat4 m_view;
    glm::mat4 m_proj;

    glm::vec4 m_lightPos;
    glm::vec4 m_lightCol;// The world-space position of the point light

    float m_ka;
    float m_kd;
    float m_ks;
    float m_shininess;

    SceneMaterial material;


};
