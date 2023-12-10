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
#include "object.h"
#include "sphere.h"

#include "shaderloader.h"
#include "../projects-realtime-AshMa1022/src/utils/sceneparser.h"


class Realtime : public QOpenGLWidget
{
public:
    Realtime(QWidget *parent=nullptr);
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

//    void bindHelper(Shape &shape,std::vector<float> &vertex, int index);

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
    GLuint m_filter;



    GLuint m_shade;
    std::vector<GLuint> m_vbos;    // Stores id of VBO
    std::vector<GLuint> m_vaos;    // Stores id of VAO
    GLuint m_fbo;
    GLuint m_fbo_texture;
    QImage m_image;
    GLuint m_cake;
    GLuint m_fbo_renderbuffer;
    GLuint m_defaultFBO;
    Object obj;
    Object candle;
    Sphere sphere;
    std::vector<GLfloat> sphere_vertex;
    std::vector<GLfloat> obj_vertex;
    std::vector<GLfloat> candle_vertex;

    int cout;
    bool initialized = false;
    bool blur = false;
    bool sharpe = false;
    bool invert = false;
    bool gray = false;
    bool pixel = false;
    bool cel_shading = false;
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

    glm::mat4 rotateX(float unit);
    void drawText(GLuint texture);
    void makeFBO();
    void bindObj(int index,Object &obj,std::vector<float> &vertex,const char* path);


};
