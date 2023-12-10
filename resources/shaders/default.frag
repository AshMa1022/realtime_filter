#version 330 core
// Task 5: declare "in" variables for the world-space position and normal,
//         received post-interpolation from the vertex shader
in vec3 position;
in vec3 normal;
in vec2 text;
// Task 10: declare an out vec4 for your output color
out vec4 color;

// Task 12: declare relevant uniform(s) here, for ambient lighting
uniform float m_ka;
uniform sampler2D samp;
uniform vec3 material[3];
// Task 13: declare relevant uniform(s) here, for diffuse lighting
uniform float m_kd;
uniform vec4 light_pos[10]; //if directional then light_pos represent light_dir
uniform vec4 light_col[10];
uniform vec4 spot[10];
uniform vec2 angle[10]; //0:angle 1:pen
uniform int light_type[10];
uniform vec3 light_func[10];
uniform bool cel_shading;

uniform int num_l;
// Task 14: declare relevant uniform(s) here, for specular lighting
uniform float m_ks;
uniform float m_shininess;
uniform vec4 cam_pos;


void main() {
    if(cel_shading){
        vec3 accumulatedAmbient = vec3(0.0);
        vec3 accumulatedDiffuse = vec3(0.0);
        int spot_cout=0;
        float att = 1;
        for(int i = 0; i < num_l; i++) {
            float dis = distance(light_pos[i],vec4(position,1.0));
            vec3 function = light_func[i];
            float a =1.0 /(function[0] + function[1] * dis + function[2] *dis * dis);
            att = (a>1)? 1:a;
            // Ambient
            float ambientStrength = 0.1;
            vec3 ambient = ambientStrength * vec3(light_col[i]);

            // Diffuse
            vec3 norm = normalize(normal);
            vec3 lightDir = normalize(vec3(light_pos[i]) - position);
            float diff = max(dot(norm, lightDir), 0.0);

            // Accumulate ambient and diffuse contributions
            accumulatedAmbient += ambient;
            accumulatedDiffuse += att*diff * vec3(light_col[i]);
        }

        if (length(accumulatedDiffuse) > 0.8)
            accumulatedDiffuse = vec3(0.8);
        else if (length(accumulatedDiffuse) > 0.65)
            accumulatedDiffuse = vec3(0.7);
        else if (length(accumulatedDiffuse) > 0.58)
            accumulatedDiffuse = vec3(0.65);
        else if (length(accumulatedDiffuse) > 0.5)
            accumulatedDiffuse = vec3(0.6);
        else if (length(accumulatedDiffuse) > 0.3)
            accumulatedDiffuse = vec3(0.4);
        else
            accumulatedDiffuse = vec3(0.2);

        // Clamp color
        vec3 final = vec3(texture(samp,text)) *(accumulatedAmbient + accumulatedDiffuse);
        color = vec4(final,1.0);
    }
//    // Remember that you need to renormalize vectors here if you want them to be normalized

else{
    color =vec4(0,0,0,0);
    color += m_ka * vec4(material[0],1.f);
    int spot_cout=0;

    // Task 12: add ambient component to output color
    for(int i =0; i<num_l; i++){
        float att;
        vec4 light_dir;
         // Ambient component
        if(light_type[i] == 0){
            att = 1;
            light_dir =-normalize(light_pos[i]);
        }
        else{
            float dis = distance(light_pos[i],vec4(position,1.0));
            vec3 function = light_func[i];
            float a =1.0 /(function[0] + function[1] * dis + function[2] *dis * dis);
            att = (a>1)? 1:a;
            light_dir =normalize(light_pos[i]-vec4(position,1.0));


            if(light_type[i] == 2){
                float between = acos(dot(normalize(vec4(position,1.0)-light_pos[i]),normalize(spot[spot_cout])));
                float inner = angle[spot_cout][0]-angle[spot_cout][1];
                float outer = angle[spot_cout][0];


                if(between >inner && between <= outer){
                    float falloff= -2 * pow((between-inner)/(angle[spot_cout][1]),3) + 3 *  pow((between-inner)/(angle[spot_cout][1]),2);
                    att= 1-falloff;

                }
                else if(between >outer){
                    att = 0;
                }
                spot_cout++;
            }
        }


        float dott = dot(light_dir,normalize(vec4(normal,0.f)));
        dott = clamp(dott, 0.f, 1.f);

        color += att* 0.8*m_kd * dott* vec4(material[1],1.f)* light_col[i]+ 0.2*(texture(samp, text)); // Diffuse component

        vec4 dir_to_cam = normalize(cam_pos-vec4(position,1.0));

        vec4 reflec = normalize(reflect(-light_dir, normalize(vec4(normal, 0.0))));
        if (m_shininess >0){
            float spec= pow(clamp(dot(dir_to_cam,reflec),0.f,1.f),m_shininess);
            color += att* m_ks * spec *vec4(material[2],1.f) * light_col[i] ; // Specular component
        }

    }
    }


}

