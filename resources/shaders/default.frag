#version 330 core
// Task 5: declare "in" variables for the world-space position and normal,
//         received post-interpolation from the vertex shader
in vec3 position;
in vec3 normal;
// Task 10: declare an out vec4 for your output color
out vec4 color;

// Task 12: declare relevant uniform(s) here, for ambient lighting
uniform float m_ka;

uniform vec3 material[3];
// Task 13: declare relevant uniform(s) here, for diffuse lighting
uniform float m_kd;
uniform vec4 light_pos[10];
uniform vec4 light_col[10];
uniform int num_l;
// Task 14: declare relevant uniform(s) here, for specular lighting
uniform float m_ks;
uniform float m_shininess;
uniform vec4 cam_pos;

void main() {
    // Remember that you need to renormalize vectors here if you want them to be normalized

    // Task 10: set your output color to white (i.e. vec4(1.0)). Make sure you get a white circle!
    color =vec4(0,0,0,0);
    color += m_ka * vec4(material[0],1.f);


    // Task 12: add ambient component to output color
    for(int i =0; i<num_l; i++){
         // Ambient component

        vec4 light_dir =-normalize(light_pos[i]);
        float dott = dot(light_dir,normalize(vec4(normal,0.f)));
        dott = clamp(dott, 0.f, 1.f);

        color += m_kd * dott* vec4(material[1],1.f)* light_col[i]; // Diffuse component

        vec4 dir_to_cam = normalize(cam_pos-vec4(position,1.0));
        vec4 reflec = reflect(-light_dir, normalize(vec4(normal, 0.0)));
        if (m_shininess >0){
            float spec = pow(clamp(dot(dir_to_cam,reflec),0.f,1.f),m_shininess);
            color += m_ks * spec *vec4(material[2],1.f) * light_col[i] ; // Specular component
        }
        else{
//            color += m_ks *vec4(material[2],1.f) * light_col[i] ;
        }
    }

}
