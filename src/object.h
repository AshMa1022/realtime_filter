#pragma once

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>


class Object
{
public:
    std::vector<GLfloat> out_vertices;
    std::vector<GLfloat> out_normals;
    bool loadOBJ(const char* path);
};


