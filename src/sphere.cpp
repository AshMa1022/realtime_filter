#include "sphere.h"


void Sphere::updateParam(int param1, int param2){
    m_vertexData = std::vector<float>();
    m_param1 = 10;
    m_param2 = 10;
    setVertexData();
}

void Sphere::makeTile(glm::vec3 topLeft,
                      glm::vec3 topRight,
                      glm::vec3 bottomLeft,
                      glm::vec3 bottomRight) {
    // Task 5: Implement the makeTile() function for a Sphere
    // Note: this function is very similar to the makeTile() function for Cube,
    //       but the normals are calculated in a different way!
    glm::vec3 sphereCenter(0.0f, 0.0f, 0.0f);
    glm::vec3 normal1 = glm::normalize(topLeft - sphereCenter);
    glm::vec3 normal2 = glm::normalize(topRight - sphereCenter);
    glm::vec3 normal3 = glm::normalize(bottomLeft - sphereCenter);
    glm::vec3 normal4 = glm::normalize(bottomRight - sphereCenter);

    // Triangle 1 (top-left, top-right, bottom-left)
    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, normal1);
    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, normal2);
    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, normal3);

    // Triangle 2 (top-right, bottom-right, bottom-left)
    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, normal2);
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, normal4);
    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, normal3);



}

void Sphere::makeWedge(float currentTheta, float nextTheta) {
    // Task 6: create a single wedge of the sphere using the
    //         makeTile() function you implemented in Task 5
    // Note: think about how param 1 comes into play here!


    // Calculate the step for phi based on the number of segments
    float stepPhi =M_PI /(float) (m_param1*m_param1);  // Assumes a full 180-degree sphere


    for (int j = 0; j < m_param1*m_param1; j++) {
        // Calculate the angles for the current phi segment
        float startPhi = j * stepPhi;
        float endPhi = (j + 1) * stepPhi;

        // Calculate vertices for the current segment using spherical coordinates
        glm::vec3 topLeft, topRight, bottomLeft, bottomRight;

        // Calculate the x, y, and z coordinates based on the angles
        float x1 = 0.5 * sin(startPhi) * cos(currentTheta);
        float y1 = 0.5 * cos(startPhi);
        float z1 = 0.5 * sin(startPhi) * sin(currentTheta);

        float x2 = 0.5 * sin(startPhi) * cos(nextTheta);
        float y2 = 0.5 * cos(startPhi);
        float z2 = 0.5 * sin(startPhi) * sin(nextTheta);

        float x3 = 0.5 * sin(endPhi) * cos(currentTheta);
        float y3 = 0.5 * cos(endPhi);
        float z3 = 0.5 * sin(endPhi) * sin(currentTheta);

        float x4 = 0.5 * sin(endPhi) * cos(nextTheta);
        float y4 = 0.5 * cos(endPhi);
        float z4 = 0.5 * sin(endPhi) * sin(nextTheta);

        // Set the coordinates for the vertices
        topLeft = glm::vec3(x1, y1, z1);
        topRight = glm::vec3(x2, y2, z2);
        bottomLeft = glm::vec3(x3, y3, z3);
        bottomRight = glm::vec3(x4, y4, z4);

        // Make a tile for the current segment
        makeTile(topLeft, topRight, bottomLeft, bottomRight);
    }


}

void Sphere::makeSphere() {
    // Task 7: create a full sphere using the makeWedge() function you
    //         implemented in Task 6
    // Note: think about how param 2 comes into play here!
    float thetaStep = glm::radians(360.f / m_param2);
    for(int i = 0; i<m_param2; i++){
        float currentTheta = i * thetaStep;
        float nextTheta = (i+1) * thetaStep;
        makeWedge(currentTheta, nextTheta);
    }
}

void Sphere::setVertexData() {
    makeSphere();
}

// Inserts a glm::vec3 into a vector of floats.
// This will come in handy if you want to take advantage of vectors to build your shape!
void Sphere::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}

