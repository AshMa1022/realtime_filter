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
/**
 * @brief Realtime::bindObj: helper method to bind the mesh
 * @param index:vbo/vao index
 * @param obj: obj instance to load and parse vertex
 * @param vertex: vertex that stored each mesh's vertex/normal/uv coord
 * @param path: path of the obj file
 */
void Realtime::bindObj(int index,Object &obj,std::vector<GLfloat> &vertex,const char* path){
    if(obj.loadOBJ(path)){
        glClearColor(0.f,0.f,0.f,0.f); //clear the state
        m_vbos[index] = 0;
        glGenBuffers(1,&m_vbos[index]);
        glBindBuffer(GL_ARRAY_BUFFER,m_vbos[index]);
        vertex = obj.out_vertices;
        glBufferData(GL_ARRAY_BUFFER,sizeof(GLfloat) *vertex.size(),vertex.data(),GL_STATIC_DRAW);
        glGenVertexArrays(1,&m_vaos[index]);
        glBindVertexArray(m_vaos[index]);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,8 *sizeof(GL_FLOAT),0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,8 *sizeof(GL_FLOAT),reinterpret_cast<void*>(3 *sizeof(GL_FLOAT)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,8 *sizeof(GL_FLOAT),reinterpret_cast<void*>(6 *sizeof(GL_FLOAT)));
        glBindBuffer(GL_ARRAY_BUFFER,0);
        glBindVertexArray(0);
    }
    else{
        std::cout<<"did not load OBJ file correctly"<<std::endl;
    }

}

/**
 * @brief Realtime::makeFBO: helper method generate FBO for post-processing filter
 */
void Realtime::makeFBO(){
    //Task 19: Generate and bind an empty texture, set its min/mag filter interpolation, then unbind
    glGenTextures(1,&m_fbo_texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,m_fbo_texture);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,size().width() * m_devicePixelRatio,size().height() * m_devicePixelRatio,0,GL_RGBA,GL_UNSIGNED_BYTE,nullptr);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D,0);

    // Task 20: Generate and bind a renderbuffer of the right size, set its format, then unbind
    glGenRenderbuffers(1,&m_fbo_renderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER,m_fbo_renderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH24_STENCIL8,size().width() * m_devicePixelRatio,size().height() * m_devicePixelRatio);
    glBindRenderbuffer(GL_RENDERBUFFER,0);

    // Task 18: Generate and bind an FBO
    glGenFramebuffers(1,&m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER,m_fbo);

    // Task 21: Add our texture as a color attachment, and our renderbuffer as a depth+stencil attachment, to our FBO
    glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,m_fbo_texture,0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_STENCIL_ATTACHMENT,GL_RENDERBUFFER,m_fbo_renderbuffer);
        // Task 22: Unbind the FBO
    glBindFramebuffer(GL_FRAMEBUFFER,m_defaultFBO);

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

    // Allows OpenGL to draw objects appropriately on top of one another
    glEnable(GL_DEPTH_TEST);
    // Tells OpenGL to only draw the front face
//    glEnable(GL_CULL_FACE);
    // Tells OpenGL how big the screen is
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);

    // Students: anything requiring OpenGL calls when the program starts should be done here
    m_shader = ShaderLoader::createShaderProgram(":/resources/shaders/default.vert",":/resources/shaders/default.frag");
    m_filter = ShaderLoader::createShaderProgram(":/resources/shaders/texture.vert",":/resources/shaders/texture.frag");
    m_particle = ShaderLoader::createShaderProgram(":/resources/shaders/particle.vert",":/resources/shaders/particle.frag");


    //load the cake texture
        // Prepare filepath
        QString kitten_filepath = QString("/Users/ash/Desktop/CS1230/Realtime_filter/resources/images/cakiiii.png");

        // Task 1: Obtain image from filepath
        m_image.load(kitten_filepath);
        std::cout<<m_image.load(kitten_filepath)<<std::endl;
        // Task 2: Format image to fit OpenGL
        m_image = m_image.convertToFormat(QImage::Format_RGBA8888).mirrored();
        // Task 3: Generate kitten texture
        glGenTextures(1,&m_cake);

        // Task 9: Set the active texture slot to texture slot 0
        glActiveTexture(GL_TEXTURE0);
        // Task 4: Bind kitten texture
        glBindTexture(GL_TEXTURE_2D,m_cake);

        // Task 5: Load image into kitten texture
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,m_image.width(),m_image.height(),0,GL_RGBA,GL_UNSIGNED_BYTE,m_image.bits());

        // Task 6: Set min and mag filters' interpolation mode to linear
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);


        // Task 7: Unbind kitten texture
        glBindTexture(GL_TEXTURE_2D,0);
        glUseProgram(m_shader);
        glUniform1i(glGetUniformLocation(m_shader,"samp"),0);
        glUseProgram(0);

    bindObj(0,obj,obj_vertex,"/Users/ash/Desktop/CS1230/Realtime_filter/cakii.obj");
    bindObj(1,candle,candle_vertex,"/Users/ash/Desktop/CS1230/Realtime_filter/candle.obj");

    part = Particles(100,m_vbos[3],m_vaos[3]);

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
 * @brief Realtime::drawText: helper method passin all the needed parameter for FBO
 * @param texture: in this case always the empty texture.
 */
void Realtime::drawText(GLuint texture){
    glBindFramebuffer(GL_FRAMEBUFFER,m_defaultFBO);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(m_filter);
    // Task 32: Set your bool uniform on whether or not to filter the texture drawn

    glBindVertexArray(m_vaos[5]);
    // Task 10: Bind "texture" to slot 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,texture);
    glUniform1i(glGetUniformLocation(m_filter,"invert"),invert);
    glUniform1i(glGetUniformLocation(m_filter,"gray"),gray);
    glUniform1i(glGetUniformLocation(m_filter,"pixel"),pixel);
    glUniform1i(glGetUniformLocation(m_filter,"blur"),blur);
    glUniform1i(glGetUniformLocation(m_filter,"dith"),settings.dith);
    glUniform1f(glGetUniformLocation(m_filter,"width"),1.f/(size().width() * m_devicePixelRatio));
    glUniform1f(glGetUniformLocation(m_filter,"height"),1.f/(size().height() * m_devicePixelRatio));
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    // Task 3: deactivate the shader program by passing 0 into glUseProgram
    glUseProgram(0);


}

void Realtime::paintGL() {
    glBindFramebuffer(GL_FRAMEBUFFER,m_fbo);
    glViewport(0,0,size().width() * m_devicePixelRatio,size().height() * m_devicePixelRatio);
    glClearColor(0.176, 0.173, 0.251, 1.0f); // Blue background
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

        glUniformMatrix4fv(glGetUniformLocation(m_shader,"model"),1,GL_FALSE,&m_model[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(m_shader,"view"),1,GL_FALSE,&m_view[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(m_shader,"projection"),1,GL_FALSE,&m_proj[0][0]);

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
        glUniform1i(glGetUniformLocation(m_shader,"num_l"),cout);
        glUniform1i(glGetUniformLocation(m_shader,"spot_cout"),spot_cout);
        glUniform3fv(glGetUniformLocation(m_shader, "material[0]"),1,&material.cAmbient[0]);
        glUniform3fv(glGetUniformLocation(m_shader, "material[1]"),1,&material.cDiffuse[0]);
        glUniform3fv(glGetUniformLocation(m_shader, "material[2]"),1,&material.cSpecular[0]);

        glUniform1f(glGetUniformLocation(m_shader,"m_ka"),m_ka);
        // Task 13: pass light position and m_kd into the fragment shader as a uniform

        glUniform1f(glGetUniformLocation(m_shader,"m_kd"),m_kd);
        // Task 14: pass shininess, m_ks, and world-space camera position
        glUniform1f(glGetUniformLocation(m_shader,"m_ks"),m_ks);
        glUniform1f(glGetUniformLocation(m_shader,"m_shininess"),m_shininess);
        glUniform1i(glGetUniformLocation(m_shader,"part"),false);

        glm::vec4 cameraPosition = glm::vec4(glm::vec3(glm::inverse(m_view)[3]),1.0);
        glUniform4fv(glGetUniformLocation(m_shader,"cam_pos"),1,&cameraPosition[0]);

        // Draw Command
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,m_cake);
        glDrawArrays(GL_TRIANGLES, 0, vertex.size() / 8);
        // Unbind Vertex Array
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);


    }
    if(settings.part){
        part.update(deltaTime);
        part.render(m_shader,m_model,m_view,m_proj,m_vaos[3]);
    }
    glUseProgram(0);


    drawText(m_fbo_texture);
    }

}


void Realtime::resizeGL(int w, int h) {
    // Tells OpenGL how big the screen is
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);
    glDeleteTextures(1,&m_fbo_texture);
    glDeleteRenderbuffers(1,&m_fbo_renderbuffer);
    glDeleteFramebuffers(1,&m_fbo);
    makeFBO();
    // Students: anything requiring OpenGL calls when the program starts should be done here
    m_proj =/*glm::perspective(glm::radians(45.0),1.0 * width() / height(),0.01,100.0);*/
             cam.getProjection(settings.nearPlane,settings.farPlane,
                               size().width() * m_devicePixelRatio,size().height() * m_devicePixelRatio);


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

// ================== Project 6: Action!

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
        int deltaY =  m_prev_mouse_pos.y -posY ;
        m_prev_mouse_pos = glm::vec2(posX, posY);

        // Use deltaX and deltaY here to rotate
        float angleX = static_cast<float>(deltaY)*0.5 ; // or -deltaY, depending on your coordinate system
        float angleY = static_cast<float>(deltaX)*0.5 ; // or -deltaX

        m_model = glm::rotate(m_model, glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));
//        m_view = cam.rotateX(angleX);
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

