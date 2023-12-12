#include "realtime.h"
#include "realtime.h"
#include <QCoreApplication>
#include <QMouseEvent>
#include <QKeyEvent>
#include <iostream>
#include "settings.h"
#include "camera.h"
#include <chrono>
#include "glm/gtc/constants.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"

Realtime::Realtime(QWidget *parent)
    : QOpenGLWidget(parent)
{
    m_prev_mouse_pos = glm::vec2(size().width()/2, size().height()/2);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);

    m_keyMap[Qt::Key_W]       = false;
    m_keyMap[Qt::Key_A]       = false;
    m_keyMap[Qt::Key_S]       = false;
    m_keyMap[Qt::Key_D]       = false;
    m_keyMap[Qt::Key_Control] = false;
    m_keyMap[Qt::Key_Space]   = false;

}
void Realtime::finish() {
    killTimer(m_timer);
    this->makeCurrent();

    // Students: anything requiring OpenGL calls when the program exits should be done here];
    glDeleteBuffers(5,&m_vbos[0]);
    glDeleteVertexArrays(5,&m_vaos[0]);
    glDeleteTextures(1,&m_fbo_texture);
    glDeleteTextures(1,&m_cake);
    glDeleteRenderbuffers(1,&m_fbo_renderbuffer);
    glDeleteFramebuffers(1,&m_fbo);
    glDeleteProgram(m_shader);
    glDeleteProgram(m_filter);
    glDeleteProgram(m_particle);


    this->doneCurrent();
}

void Realtime::initializeGL() {
    m_devicePixelRatio = this->devicePixelRatio();
    m_timer = startTimer(1000/60);
    m_elapsedTimer.start();
    m_vbos={0,0,0,0};
    m_vaos={0,0,0,0};
    m_defaultFBO = 2;

    // Initializing GL.
    // GLEW (GL Extension Wrangler) provides access to OpenGL functions.
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "Error while initializing GL: " << glewGetErrorString(err) << std::endl;
    }
    std::cout << "Initialized GL: Version " << glewGetString(GLEW_VERSION) << std::endl;

    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);

    // Students: anything requiring OpenGL calls when the program starts should be done here
    m_shader = ShaderLoader::createShaderProgram(":/resources/shaders/default.vert",":/resources/shaders/default.frag");
    m_filter = ShaderLoader::createShaderProgram(":/resources/shaders/texture.vert",":/resources/shaders/texture.frag");


    //load the cake texture
        QString kitten_filepath = QString("/Users/ash/Desktop/CS1230/Realtime_filter/resources/images/cakiiii.png");
        m_image.load(kitten_filepath);
        m_image = m_image.convertToFormat(QImage::Format_RGBA8888).mirrored();
        glGenTextures(1,&m_cake);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,m_cake);
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,m_image.width(),m_image.height(),0,GL_RGBA,GL_UNSIGNED_BYTE,m_image.bits());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D,0);
        glUseProgram(m_shader);
        glUniform1i(glGetUniformLocation(m_shader,"samp"),0);
        glUseProgram(0);

    bindObj(0,obj,obj_vertex,"/Users/ash/Desktop/CS1230/Realtime_filter/cakii.obj");
    bindObj(1,candle,candle_vertex,"/Users/ash/Desktop/CS1230/Realtime_filter/candle.obj");

    part = Particles(100); // load the fallen ribbon fragment

    std::vector<GLfloat> fullscreen_quad_data =
        { //     POSITIONS    //
            -1.f,  1.f, 0.0f,
            0.f,  1.f,  0.f, //uv1
            -1.f, -1.f, 0.0f,
            0.f,  0.f, 0.f,  //uv2
            1.f, -1.f, 0.f,
            1.f,  0.f, 0.f,  //uv3
            1.f,  1.f, 0.0f,
            1.f,  1.f, 0.f, //uv4
            -1.f,  1.f, 0.0f,
            0.f,  1.f,  0.f, //uv5
            1.f, -1.f, 0.0f,
            1.f,  0.f, 0.f,  //uv6
        };
    glGenBuffers(1, &m_vbos[5]);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbos[5]);
    glBufferData(GL_ARRAY_BUFFER, fullscreen_quad_data.size()*sizeof(GLfloat), fullscreen_quad_data.data(), GL_STATIC_DRAW);
    glGenVertexArrays(1, &m_vaos[5]);
    glBindVertexArray(m_vaos[5]);

    // Task 14: modify the code below to add a second attribute to the vertex attribute array
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void*>(3 *sizeof(GL_FLOAT)));

    // Unbind the fullscreen quad's VBO and VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    makeFBO();

}



/**
 * @brief Realtime::paintGL
 */
void Realtime::paintGL() {
    glBindFramebuffer(GL_FRAMEBUFFER,m_fbo);
    glViewport(0,0,size().width() * m_devicePixelRatio,size().height() * m_devicePixelRatio);
    glClearColor(1, 0.718, 0.718, 1.0f); // Blue background
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if(initialized){
        auto currentFrameTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(currentFrameTime - lastFrameTime).count();
        lastFrameTime = currentFrameTime;
        glUseProgram(m_shader);
        glUniform1i(glGetUniformLocation(m_shader,"cel_shading"),cel_shading);

    //    // Bind Sphere Vertex Data
        for(int i=0; i< shapes.size();i++){

            m_shininess = settings.m_data.shapes[i].primitive.material.shininess;
            std::vector<float> vertex;
            if(shapes[i].primitive.meshfile=="/Users/ash/Desktop/CS1230/caki"){
                vertex=obj_vertex;
                glBindVertexArray(m_vaos[0]);
            }
            else if(shapes[i].primitive.meshfile=="/Users/ash/Desktop/CS1230/candle"){
                vertex=candle_vertex;
                glBindVertexArray(m_vaos[1]);
            }

            material = shapes[i].primitive.material;
            //phong
            cout=lights.size();
            int spot_cout = 0;
            for(int l = 0; l<lights.size();l++){


                std::string type = "light_type[" + std::to_string(l) + "]";
                std::string name = "light_pos[" + std::to_string(l) + "]";
                std::string func = "light_func[" + std::to_string(l) + "]";
                std::string spot = "spot[" + std::to_string(spot_cout) + "]";
                std::string angle = "angle[" + std::to_string(spot_cout) + "]";
                glm::vec2 angles;


                int loc;
                int loc_f;

                switch(lights[l].type){
                case(LightType::LIGHT_DIRECTIONAL):
                    glUniform1i(glGetUniformLocation(m_shader,type.c_str()),0);
                    loc = glGetUniformLocation(m_shader,name.c_str());
                    glUniform4fv(loc,1,&lights[l].dir[0]);
                    break;
                case(LightType::LIGHT_POINT):
                    glUniform1i(glGetUniformLocation(m_shader,type.c_str()),1);
                    loc = glGetUniformLocation(m_shader,name.c_str());
                    loc_f = glGetUniformLocation(m_shader, func.c_str());
                    glUniform4fv(loc,1,&lights[l].pos[0]);
                    glUniform3fv(loc_f,1,&lights[l].function[0]);
                    break;
                case(LightType::LIGHT_SPOT):
                    glUniform1i(glGetUniformLocation(m_shader,type.c_str()),2);
                    loc = glGetUniformLocation(m_shader,name.c_str());
                    loc_f = glGetUniformLocation(m_shader, func.c_str());
                    glUniform4fv(loc,1,&lights[l].pos[0]);
                    glUniform4fv(glGetUniformLocation(m_shader, spot.c_str()),1,&lights[l].dir[0]);
                    angles[0] = lights[l].angle;
                    angles[1] = lights[l].penumbra;
                    glUniform2fv(glGetUniformLocation(m_shader, angle.c_str()),1,&angles[0]);
                    glUniform3fv(loc_f,1,&lights[l].function[0]);
                    spot_cout ++;
                    break;
                default:
                    std::cout<<"wrong"<<std::endl;
                    break;
                }


                std::string name2 = "light_col[" + std::to_string(l) + "]";
                int loc2 = glGetUniformLocation(m_shader,name2.c_str());
                glUniform4fv(loc2,1,&lights[l].color[0]);

            }
            glUniform1i(glGetUniformLocation(m_shader,"spot_cout"),spot_cout);
            passer();
            // Draw Command
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D,m_cake);
            glDrawArrays(GL_TRIANGLES, 0, vertex.size() / 8);
            // Unbind Vertex Array
            glBindVertexArray(0);
            glBindTexture(GL_TEXTURE_2D, 0);


        }
        if(settings.part){ // whether there is fragment falling
            part.update(deltaTime);
            part.render(m_shader,m_model,m_view,m_proj,m_vaos[3]);
        }
        float angleY = static_cast<float>(deltaTime)*15 ; // or -deltaX
        m_model = glm::rotate(m_model, glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));
        glUseProgram(0);
        drawText(m_fbo_texture);
    }

}



void Realtime::sceneChanged() {

    update(); // asks for a PaintGL() call to occur
    //load Camera
    cam = Camera(settings.m_data.cameraData);
    m_view =/*glm::lookAt(glm::vec3(2.f,0,0),glm::vec3(0,0,0),glm::vec3(0,0,1))*/
             cam.getViewMatrix();
    m_proj =glm::perspective(glm::radians(45.0),1.0 * width() / height(),0.01,100.0);


    //load shapes and light
    shapes = settings.m_data.shapes;
    lights = settings.m_data.lights;
    globalData = settings.m_data.globalData;
    m_ka =globalData.ka;
    m_ks =globalData.ks;
    m_kd =globalData.kd;
    initialized = true;
    cout=0;
    m_model= glm::mat4(1.f);
    lastFrameTime = std::chrono::high_resolution_clock::now();

}


void Realtime::settingsChanged() {
    if(initialized){

        update(); // asks for a PaintGL() call to occur
        m_proj =glm::perspective(glm::radians(45.0),1.0 * width() / height(),0.01,100.0);
        pixel =(settings.kernelBasedFilter==true)?true:false;
        blur =(settings.perPixelFilter==true)?true:false;
        cel_shading =(settings.cel==true)?true:false;


    }
}


void Realtime::keyPressEvent(QKeyEvent *event) {
    m_keyMap[Qt::Key(event->key())] = true;
}

void Realtime::keyReleaseEvent(QKeyEvent *event) {
    m_keyMap[Qt::Key(event->key())] = false;
}

void Realtime::mousePressEvent(QMouseEvent *event) {
    if (event->buttons().testFlag(Qt::LeftButton)) {
        m_mouseDown = true;
        m_prev_mouse_pos = glm::vec2(event->position().x(), event->position().y());
    }
}

void Realtime::mouseReleaseEvent(QMouseEvent *event) {
    if (!event->buttons().testFlag(Qt::LeftButton)) {
        m_mouseDown = false;
    }
}

void Realtime::mouseMoveEvent(QMouseEvent *event) {
    if (m_mouseDown) {
        int posX = event->position().x();
        int posY = event->position().y();
        int deltaX = m_prev_mouse_pos.x -posX ;
        m_prev_mouse_pos = glm::vec2(posX, posY);

        // Use deltaX and deltaY here to rotate
        float angleY = static_cast<float>(deltaX)*0.5 ; // or -deltaX

        m_model = glm::rotate(m_model, glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));
        update(); // asks for a PaintGL() call to occur
    }
}

void Realtime::timerEvent(QTimerEvent *event) {
    int elapsedms   = m_elapsedTimer.elapsed();
    float deltaTime = elapsedms * 0.001f;
    m_elapsedTimer.restart();


    // Use deltaTime and m_keyMap here to move around

    if(m_keyMap[Qt::Key_W]){
        float translationAmount = 5.0f * deltaTime;
        m_view = cam.getTranslateView(translationAmount);
    }
    if(m_keyMap[Qt::Key_S]){
        float translationAmount = -5.0f * deltaTime;
        m_view = cam.getTranslateView(translationAmount);
    }
    if(m_keyMap[Qt::Key_A]){
        float translationAmount = 5.0f * deltaTime;
        m_view = cam.getTranslateAD(translationAmount);
    }
    if(m_keyMap[Qt::Key_D]){
        float translationAmount = -5.0f * deltaTime;
        m_view = cam.getTranslateAD(translationAmount);
    }
    if(m_keyMap[Qt::Key_Space]){
        float translationAmount = 5.0f * deltaTime;
        m_view = cam.getTranslateY(translationAmount);
    }
    if(m_keyMap[Qt::Key_Control]){
        float translationAmount = -5.0f * deltaTime;
        m_view = cam.getTranslateY(translationAmount);
    }



    update(); // asks for a PaintGL() call to occur
}

// DO NOT EDIT
void Realtime::saveViewportImage(std::string filePath) {
    // Make sure we have the right context and everything has been drawn
    makeCurrent();

    int fixedWidth = 1024;
    int fixedHeight = 768;

    // Create Frame Buffer
    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // Create a color attachment texture
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, fixedWidth, fixedHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

    // Optional: Create a depth buffer if your rendering uses depth testing
    GLuint rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, fixedWidth, fixedHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Error: Framebuffer is not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return;
    }

    // Render to the FBO
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, fixedWidth, fixedHeight);

    // Clear and render your scene here
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    paintGL();

    // Read pixels from framebuffer
    std::vector<unsigned char> pixels(fixedWidth * fixedHeight * 3);
    glReadPixels(0, 0, fixedWidth, fixedHeight, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

    // Unbind the framebuffer to return to default rendering to the screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Convert to QImage
    QImage image(pixels.data(), fixedWidth, fixedHeight, QImage::Format_RGB888);
    QImage flippedImage = image.mirrored(); // Flip the image vertically

    // Save to file using Qt
    QString qFilePath = QString::fromStdString(filePath);
    if (!flippedImage.save(qFilePath)) {
        std::cerr << "Failed to save image to " << filePath << std::endl;
    }

    // Clean up
    glDeleteTextures(1, &texture);
    glDeleteRenderbuffers(1, &rbo);
    glDeleteFramebuffers(1, &fbo);
}

