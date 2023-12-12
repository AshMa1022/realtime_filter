#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>

#include <unordered_map>
#include <QElapsedTimer>
#include <QOpenGLWidget>
#include <QTime>
#include <QTimer>


class Particles
{
    struct Particle {
        glm::vec3 position=glm::vec3(0.f);
        glm::vec3 velocity;
        glm::vec4 color;
        float lifespan;
        bool alive=true;
        float size;
    };
public:
    Particles(int maxParticles);
    Particles();
    void update(float deltaTime);
    void render(GLuint shader,
                glm::mat4 m_model,
                glm::mat4 m_view,
                glm::mat4 m_proj,
                GLuint m_vao);
    void setPosition(int index, glm::vec3 v,float dealtaTime);
private:
    std::vector<Particle> particles;
    std::vector<GLfloat> position;
    int maxParticles;
    GLuint vbo;
    GLuint vao;
    void renew(Particle &particle);
};


