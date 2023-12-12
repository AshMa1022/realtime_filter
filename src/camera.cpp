#include <stdexcept>
#include "camera.h"
#include "utils/scenefilereader.h"
#include <glm/glm.hpp>
#include <iostream>

Camera::Camera(SceneCameraData c){
    pos = c.pos;
    look = c.look;
    up = c.up;
    hangle = c.heightAngle;
}

Camera::Camera(){}
glm::mat4 Camera::getViewMatrix() const{
    glm::mat4 translation(1.f,0.f,0.f,0.f,
                          0.f,1.f,0.f,0.f,
                          0.f,0.f,1.f,0.f,
                          -pos[0],-pos[1],-pos[2],1.f);

    glm::vec3 w = -glm::normalize(look);
    glm::vec3 v = glm::normalize(up - glm::dot(up,w) * w);
    glm::vec3 u = glm::cross(v,w);
    glm::mat4 rotate(u[0],v[0],w[0],0.f,
                     u[1],v[1],w[1],0.f,
                     u[2],v[2],w[2],0.f,
                     0.f,0.f,0.f,1.f);
    return rotate * translation;
}

glm::mat4 Camera::getTranslateView(float unit){
    pos += unit * glm::normalize(look);
    return getViewMatrix();
}
glm::mat4 Camera::getTranslateAD(float unit){
    pos -= unit * glm::normalize(glm::cross(look,up));
    return getViewMatrix();
}
glm::mat4 Camera::getTranslateY(float unit){
    pos[1] += unit;
    return getViewMatrix();
}
glm::mat4 Camera::rotateX(float unit){
    glm::vec3 u(0.f,1.f,0.f);
    float angle = unit*0.01;
    float c = glm::cos(angle);
    glm::mat4 rotationMatrix(c + pow(u[0],2)*(1-c),u[0]*u[1]*(1-c) + u[2]*sin(angle),u[0]*u[2]*(1-c) - u[1]*sin(angle),0.f,
                             u[0]*u[1]*(1-c) - u[2]*sin(angle),c + pow(u[1],2)*(1-c), u[1]*u[2]*(1-c) + u[0]*sin(angle),0.f,
                             u[0]*u[2]*(1-c) + u[1]*sin(angle),u[1]*u[2]*(1-c) - u[0]*sin(angle),c + pow(u[2],2)*(1-c),0.f,
                             0.f,0.f,0.f,1.f);

    look = glm::vec3(rotationMatrix * glm::vec4(look, 1.0f));
    up = glm::vec3(rotationMatrix * glm::vec4(up, 1.0f));
    return getViewMatrix();
}
glm::mat4 Camera::rotateY(float unit){
    glm::vec3 u = glm::normalize(glm::cross(look,up));
    float angle = unit*0.01;
    float c = glm::cos(angle);
    glm::mat4 rotationMatrix(c + pow(u[0],2)*(1-c),u[0]*u[1]*(1-c) + u[2]*sin(angle),u[0]*u[2]*(1-c) - u[1]*sin(angle),0.f,
                             u[0]*u[1]*(1-c) - u[1]*sin(angle),c + pow(u[1],2)*(1-c), u[1]*u[2]*(1-c) + u[0]*sin(angle),0.f,
                             u[0]*u[2]*(1-c) + u[1]*sin(angle),u[1]*u[2]*(1-c) - u[0]*sin(angle),c + pow(u[2],2)*(1-c),0.f,
                             0.f,0.f,0.f,1.f);
    look = glm::vec3(rotationMatrix * glm::vec4(look, 1.0f));
    up = glm::vec3(rotationMatrix * glm::vec4(up, 1.0f));
    return getViewMatrix();
}

glm::mat4 Camera::getInverse(glm::mat4 view) const{
    return glm::inverse(view);
}

//float Camera::getHalfAngle() const {
//    return hangle / 2.0;
//}
glm::mat4 Camera::getProjection(float near,float far,int width, int height){
    float aspectRatio = width / (float)height;
    float scaleX = 1.f/(far*tan((hangle*aspectRatio)/2.0));
    float scaleY = 1.f/(far*tan(hangle/2.0));
    std::cout<< scaleX <<std::endl;
    glm::mat4 scale(  scaleX,0.f,0.f,0.f,
                    0.f,scaleY,0.f,0.f,
                    0.f,0.f,1.f/far,0.f,
                    0.f,0.f,0.f,1.f);
    float c = -near/(float)far;
    glm::mat4 pp(   1.f,0.f,0.f,0.f,
                 0.f,1.f,0.f,0.f,
                 0.f,0.f,1.f/(1+c),-c/(1+c),
                 0.f,0.f,-1.f,0.f);

    glm::mat4 map(  1.f,0.f,0.f,0.f,
                  0.f,1.f,0.f,0.f,
                  0.f,0.f,-2.f,-1.f,
                  0.f,0.f,0.f,1.f);
    return glm::transpose(map) * glm::transpose(pp) * scale;


}
