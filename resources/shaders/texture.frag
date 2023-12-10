#version 330 core

// Task 16: Create a UV coordinate in variable
in vec3 uv;

// Task 8: Add a sampler2D uniform
uniform sampler2D samp;
uniform float width;
uniform float height;

// Task 29: Add a bool on whether or not to filter the texture
uniform bool invert;
uniform bool pixel;
uniform bool blur;
uniform bool gray;
out vec4 fragColor;

void main()
{
//    fragColor = vec4(1);
    // Task 17: Set fragColor using the sampler2D at the UV coordinate
    fragColor = texture(samp,vec2(uv));

    if(blur){

        float bk[81] = float[81](
           1/55.f,1/55.f,1/55.f,1/55.f,1/55.f,1/55.f,1/55.f,1/55.f,1/55.f,
                    1/55.f,1/55.f,1/55.f,1/55.f,1/55.f,1/55.f,1/55.f,1/55.f,1/55.f,
                    1/55.f,1/55.f,1/55.f,1/55.f,1/55.f,1/55.f,1/55.f,1/55.f,1/55.f,
                    1/55.f,1/55.f,1/55.f,1/55.f,1/55.f,1/55.f,1/55.f,1/55.f,1/55.f,
                    1/55.f,1/55.f,1/55.f,1/55.f,1/55.f,1/55.f,1/55.f,1/55.f,1/55.f,
                    1/55.f,1/55.f,1/55.f,1/55.f,1/55.f,1/55.f,1/55.f,1/55.f,1/55.f,
                    1/55.f,1/55.f,1/55.f,1/55.f,1/55.f,1/55.f,1/55.f,1/55.f,1/55.f,
                    1/55.f,1/55.f,1/55.f,1/55.f,1/55.f,1/55.f,1/55.f,1/55.f,1/55.f,
                    1/55.f,1/55.f,1/55.f,1/55.f,1/55.f,1/55.f,1/55.f,1/55.f,1/55.f);
        // Normalize the kernel
        for (int i = 0; i < 81; ++i) {
            bk[i] /= 1115.0;
        }

         vec3 pix[81];
         for (int i = 0; i < 9; i++) {
             for (int j = 0; j < 9; j++) {
                 vec2 text = vec2(uv) + vec2((j - 4)*width,(i - 4)*height);
                 pix[i * 9 + j] = texture(samp, text).rgb;
             }
         }

         vec3 color = vec3(0.0);
         for (int k = 0; k < 81; k++) {

             color += pix[k] * bk[k];

         }


        fragColor = vec4(color, 1.0);

    }
    // Task 33: Invert fragColor's r, g, and b color channels if your bool is true
//    if(invert){
////        fragColor = vec4(1.f)-fragColor;
//    }
    if(pixel){
        float pix = 0.299 * fragColor[0] + 0.587 * fragColor[1] + 0.114 * fragColor[2];
        fragColor = vec4(vec3(pix),1);

        vec2 pixelatedCoords = floor(vec2(uv) / 0.01) * 0.01;
            pixelatedCoords += 0.01 * 0.5;
            vec4 texColor = texture(samp, pixelatedCoords);
            fragColor = texColor;

    }



}
