#version 330 core

// Task 16: Create a UV coordinate in variable
in vec3 uv;

// Task 8: Add a sampler2D uniform
uniform sampler2D samp;
uniform float width;
uniform float height;

// Task 29: Add a bool on whether or not to filter the texture
uniform bool invert;
uniform bool sharpe;
uniform bool blur;
uniform bool gray;
out vec4 fragColor;

void main()
{
//    fragColor = vec4(1);
    // Task 17: Set fragColor using the sampler2D at the UV coordinate
    fragColor = texture(samp,vec2(uv));
    if(sharpe){
        mat3 kernel = mat3(-1.0/9.0, -1.0/9.0, -1.0/9.0,
                            -1.0/9.0,  17.0/9.0, -1.0/9.0,
                            -1.0/9.0, -1.0/9.0, -1.0/9.0);

         vec3 pix[9];
         for (int i = 0; i < 3; i++) {
             for (int j = 0; j < 3; j++) {
                 vec2 text = vec2(uv) + vec2((j - 1)*width,(i - 1)*height);
                 pix[i * 3 + j] = texture(samp, text).rgb;
             }
         }

         vec3 color = vec3(0.0);
         for (int k = 0; k < 3; k++) {
             for(int a = 0;a < 3;a++){
             color += pix[k] * kernel[k][a];
             }
         }

        fragColor = vec4(color, 1.0);
    }
    if(invert){

        float bk[25] = float[25](1.0/25.0, 1.0/25.0, 1.0/25.0,1.0/25.0, 1.0/25.0,
                            1.0/25.0, 1.0/25.0, 1.0/25.0,1.0/25.0, 1.0/25.0,
                            1.0/25.0, 1.0/25.0, 1.0/25.0,1.0/25.0, 1.0/25.0,
                            1.0/25.0, 1.0/25.0, 1.0/25.0,1.0/25.0, 1.0/25.0,
                            1.0/25.0, 1.0/25.0, 1.0/25.0,1.0/25.0, 1.0/25.0);

         vec3 pix[25];
         for (int i = 0; i < 5; i++) {
             for (int j = 0; j < 5; j++) {
                 vec2 text = vec2(uv) + vec2((j - 2)*width,(i - 2)*height);
                 pix[i * 5 + j] = texture(samp, text).rgb;
             }
         }

         vec3 color = vec3(0.0);
         for (int k = 0; k < 25; k++) {

             color += pix[k] * bk[k];

         }

        fragColor = vec4(color, 1.0);

    }
    // Task 33: Invert fragColor's r, g, and b color channels if your bool is true
    if(invert){
//        fragColor = vec4(1.f)-fragColor;
    }
    if(gray){
        float pix = 0.299 * fragColor[0] + 0.587 * fragColor[1] + 0.114 * fragColor[2];
        fragColor = vec4(vec3(pix),1);
    }



}
