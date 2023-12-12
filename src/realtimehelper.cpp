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

void Realtime::passer(){
    glUniform1i(glGetUniformLocation(m_shader,"num_l"),cout);
    glUniform3fv(glGetUniformLocation(m_shader, "material[0]"),1,&material.cAmbient[0]);
    glUniform3fv(glGetUniformLocation(m_shader, "material[1]"),1,&material.cDiffuse[0]);
    glUniform3fv(glGetUniformLocation(m_shader, "material[2]"),1,&material.cSpecular[0]);
    glUniform1f(glGetUniformLocation(m_shader,"m_ka"),m_ka);
    glUniform1f(glGetUniformLocation(m_shader,"m_kd"),m_kd);
    glUniform1f(glGetUniformLocation(m_shader,"m_ks"),m_ks);
    glUniform1f(glGetUniformLocation(m_shader,"m_shininess"),m_shininess);
    glUniform1i(glGetUniformLocation(m_shader,"part"),false);
    glUniformMatrix4fv(glGetUniformLocation(m_shader,"model"),1,GL_FALSE,&m_model[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(m_shader,"view"),1,GL_FALSE,&m_view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(m_shader,"projection"),1,GL_FALSE,&m_proj[0][0]);
    glm::vec4 cameraPosition = glm::vec4(glm::vec3(glm::inverse(m_view)[3]),1.0);
    glUniform4fv(glGetUniformLocation(m_shader,"cam_pos"),1,&cameraPosition[0]);
}
