#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

out vec3 position;

void main() {
    position = vec3(model * vec4(aPos, 1.0));
    gl_Position = projection *view * model * vec4(aPos, 1.0);
//    gl_PointSize = 50.0; // Size of the particle
}
