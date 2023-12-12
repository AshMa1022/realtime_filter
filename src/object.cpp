#include "object.h"


/**
 * @brief Object::loadOBJ: this is the OBJ loader
 * @param path:: in put path of the file
 * @return :: whether the object file has been parsed correctly
 */
bool Object::loadOBJ(const char* path) {
    std::vector<GLuint> vertexIn, normalIn,textureIn; // store the vertex/normal/uv index in the face
    std::vector<GLfloat> temp_vertices, temp_normals,temp_texture; // store the vertex/normal/uv coords

    std::ifstream in(path, std::ios::in);
    if (!in) {
        std::cerr << "Cannot open " << path << std::endl;
        return false;
    }

    std::string line;
    while (getline(in, line)) {
        if (line.substr(0, 2) == "v ") { // stored vertex
            std::istringstream v(line.substr(2));
            GLfloat x, y, z;
            v >> x; v >> y; v >> z;
            temp_vertices.push_back(x);
            temp_vertices.push_back(y);
            temp_vertices.push_back(z);
        } else if (line.substr(0, 3) == "vn ") { // stored normal
            std::istringstream vn(line.substr(3));
            GLfloat x, y, z;
            vn >> x; vn >> y; vn >> z;
            temp_normals.push_back(x);
            temp_normals.push_back(y);
            temp_normals.push_back(z);
        }else if (line.substr(0, 3) == "vt ") { // stored uv coord
            std::istringstream vn(line.substr(3));
            GLfloat x, y;
            vn >> x; vn >> y;
            temp_texture.push_back(x);
            temp_texture.push_back(y);
        } else if (line.substr(0, 2) == "f ") { // stored faces
            std::istringstream f(line.substr(2));
            std::string segment;
            GLuint vertexIndex, textureIndex, normalIndex;
            char slash;

            while (std::getline(f, segment, ' ')) { // Split by space to get each vertex/texture/normal group
                std::istringstream faceSegment(segment);
                if (faceSegment >> vertexIndex >> slash >> textureIndex >> slash >> normalIndex) {
                    vertexIn.push_back(vertexIndex);
                    textureIn.push_back(textureIndex);
                    normalIn.push_back(normalIndex);
                }
            }
        }
    }

    for (unsigned int i = 0; i < vertexIn.size(); i++) { //push v,vn,vt base on the face indexing
        GLuint vertexIndex = vertexIn[i];
        GLfloat x = temp_vertices[3 * (vertexIndex-1)];
        GLfloat y = temp_vertices[3 * (vertexIndex-1) + 1];
        GLfloat z = temp_vertices[3 * (vertexIndex-1) + 2];


        GLuint normalIndex = normalIn[i];
        GLfloat nx = temp_normals[3 * (normalIndex-1)];
        GLfloat ny = temp_normals[3 * (normalIndex-1) + 1];
        GLfloat nz = temp_normals[3 * (normalIndex-1) + 2];

        GLuint texturelIndex = textureIn[i];
        GLfloat tx = temp_texture[2 * (texturelIndex-1)];
        GLfloat ty = temp_texture[2 * (texturelIndex-1) + 1];

        // push vertex & normal & texture coord in the same float vector so can be used later in rendering.
        out_vertices.push_back(x);
        out_vertices.push_back(y);
        out_vertices.push_back(z);
        out_vertices.push_back(nx);
        out_vertices.push_back(ny);
        out_vertices.push_back(nz);
        out_vertices.push_back(tx);
        out_vertices.push_back(ty);
    }

    return true;
}
