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
uniform bool dith;
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

    if(dith){
        float bayerMatrix[16] = float[16](
                    0.0625, 0.9375, 0.25,   0.8125,
                    0.6875, 0.375,  0.875,  0.5625,
                    0.1875, 0.75,   0.125,  1.0,
                    0.8125, 0.5,    0.625,  0.4375
        );
        vec3 col=texture(samp, vec2(uv)).rgb;
        if(pixel){
            vec2 pixelatedCoords = floor(vec2(uv) / 0.005) * 0.005;
            pixelatedCoords += 0.005 * 0.5;
            col = texture(samp, pixelatedCoords).rgb;
        }
            float gray = 0.299 * col.r + 0.587 * col.g + 0.114 * col.b;
            int x = int(mod(gl_FragCoord.x, 4));
            int y = int(mod(gl_FragCoord.y, 4));
            float ditherThreshold = bayerMatrix[y * 4 + x];

            if (gray < ditherThreshold+0.4) {
                fragColor = vec4(0.0, 0.0, 0.0, 1.0); // Black
            } else {
                fragColor = vec4(1.0, 1.0, 1.0, 1.0); // White
            }

    }



}
