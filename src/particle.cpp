#include "particle.h"
#include <iostream>
#include <random>

Particles::Particles(int maxParticles,GLuint &m_vbo,GLuint &m_vao)
{
    particles.resize(maxParticles);
    std::random_device rd;  // Obtain a random number from hardware
    std::mt19937 eng(rd()); // Seed the generator
    std::uniform_real_distribution<> distr(-3.0, 3.0); // Define the range
    std::uniform_real_distribution<> distr2(-0.3, 0.3);
    std::uniform_real_distribution<> distr3(1, 5); // Define the range


    glClearColor(0.f,0.f,0.f,0.f);
    vbo=0;


    for(auto &particle:particles){
        particle.position=glm::vec3(distr(eng),distr(eng),distr(eng));
        particle.alive=true;
        particle.lifespan=distr3(eng);
        position.push_back(particle.position[0]);
        position.push_back(particle.position[1]);
        position.push_back(particle.position[2]);
        position.push_back(particle.position[0]);
        position.push_back(particle.position[1]+distr2(eng));
        position.push_back(particle.position[2]);
        position.push_back(particle.position[0]+distr2(eng));
        position.push_back(particle.position[1]);
        position.push_back(particle.position[2]);
    }


    // Generate VBO and VAO
    glGenBuffers(1, &vbo);
    glGenVertexArrays(1, &vao);

    // Bind VAO
    glBindVertexArray(vao);

    // Bind VBO and upload data
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, position.size() * sizeof(GLfloat), position.data(), GL_STATIC_DRAW);

    // Enable and set vertex attribute pointers
    // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3 *sizeof(GL_FLOAT),0);



    // Unbind VBO and VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

}
Particles::Particles(){}

void Particles::renew(Particle &particle) {
    std::random_device rd;  // Random
    std::mt19937 eng(rd());
    std::uniform_real_distribution<> distr(1, 5.0);
    std::uniform_real_distribution<> distr2(-3, 3.0);
    particle.position = glm::vec3(distr2(eng),3,distr2(eng));
    particle.velocity = glm::vec3(0.f,-0.1f,0.f);
    particle.lifespan = distr(eng);
    particle.alive = true;
}

void Particles::update(float deltaTime) {
    int cout=0;
    std::random_device rd;
    std::mt19937 eng(rd());
    std::uniform_real_distribution<> distr2(-0.2, 0.2);
    for (auto &particle : particles) {
        if (particle.alive) {
            particle.lifespan -= deltaTime;
            particle.velocity+=glm::vec3(0.0f,-1.f, 0.0f) * (float)deltaTime * 0.5f;
            particle.position += particle.velocity * deltaTime;
            int baseIndex = cout * 9; // 3 vertices per triangle, 3 components per vertex
            position[baseIndex] += particle.velocity[0] * deltaTime;
            position[baseIndex + 1] += particle.velocity[1] * deltaTime;
            position[baseIndex + 2] += particle.velocity[2] * deltaTime;
            position[baseIndex + 3]  += particle.velocity[0] * deltaTime; // Adjust this for actual triangle shape
            position[baseIndex + 4] += particle.velocity[1] * deltaTime; // Adjust Y for the second vertex
            position[baseIndex + 5] += particle.velocity[2] * deltaTime;
            position[baseIndex + 6] += particle.velocity[0] * deltaTime;  // Adjust X for the third vertex
            position[baseIndex + 7] += particle.velocity[1] * deltaTime;
            position[baseIndex + 8] += particle.velocity[2] * deltaTime;
            if (particle.lifespan <= 0) {
                renew(particle);
                position[baseIndex] = particle.position[0];
                position[baseIndex + 1] = particle.position[1];
                position[baseIndex + 2] = particle.position[2];
                position[baseIndex + 3]  = particle.position[0]+ distr2(eng); // Adjust this for actual triangle shape
                position[baseIndex + 4] = particle.position[1]; // Adjust Y for the second vertex
                position[baseIndex + 5] = particle.position[2];
                position[baseIndex + 6] = particle.position[0];  // Adjust X for the third vertex
                position[baseIndex + 7] = particle.position[1]+ distr2(eng);
                position[baseIndex + 8] = particle.position[2];

            }
        }
        cout++;
    }
    std::cout<<position[0]<<std::endl;

    // Update the buffer data
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, position.size() * sizeof(GLfloat), position.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);

}

void Particles::render(GLuint shader, glm::mat4 m_model, glm::mat4 m_view, glm::mat4 m_proj,GLuint m_vao) {
    glBindVertexArray(vao);
    glUniform1i(glGetUniformLocation(shader,"part"),true);
    glUniformMatrix4fv(glGetUniformLocation(shader,"model"),1,GL_FALSE,&m_model[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader,"view"),1,GL_FALSE,&m_view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader,"projection"),1,GL_FALSE,&m_proj[0][0]);
    glDrawArrays(GL_TRIANGLES, 0, position.size()/3); // 3 vertices per particle
    glBindVertexArray(0);
    glUseProgram(0);
}

