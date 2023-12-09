#version 330 core
// declare a vec3 object-space position variable, using
//         the `layout` and `in` keywords.
layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec2 texture;

// declare `out` variables for the world-space position and normal,
//         to be passed to the fragment shader
out vec3 position;
out vec3 normal;
out vec2 text;

//declare a uniform mat4 to store model matrix
uniform mat4 model;

// declare uniform mat4's for the view and projection matrix
uniform mat4 view;
uniform mat4 projection;

void main() {

    text= texture;
    position = vec3(model * vec4(pos, 1.0));
    normal = normalize(transpose(inverse(mat3(model))) * norm);
    gl_Position = projection *view * model * vec4(pos, 1.0);

}
