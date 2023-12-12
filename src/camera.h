#pragma once

#include <glm/glm.hpp>
#include <utils/scenedata.h>
#include <glm/glm.hpp>
// A class representing a virtual camera.

class Camera {
public:
    Camera(SceneCameraData c);
    Camera();
    glm::mat4 view;
    glm::vec3 pos;
    glm::vec3 look;
    glm::vec3 up;
    float hangle;
    float wangle;


    glm::mat4 getViewMatrix() const;
    glm::mat4 getTranslateView(float unit);
    glm::mat4 getTranslateAD(float unit);
    glm::mat4 getTranslateY(float unit);
    glm::mat4 rotateX(float unit);
    glm::mat4 rotateY(float unit);

    glm::mat4 getInverse(glm::mat4) const;
    glm::mat4 getProjection(float far,float near, int width, int height);

    // Returns the aspect ratio of the camera.
    float getAspectRatio() const;

    // Returns the height angle of the camera in RADIANS.
    float getHalfAngle() const;


};
