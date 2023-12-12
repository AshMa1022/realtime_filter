# realtime_filter
Realtime_filter(no one cares) is Ash’s final project for CS1230 introduction to computer graphics. This project implements a cake scene(object) with phong lightning, cel shading, three post-processing filters, and a semi-particle system of falling ribbon fragments.

<i>preview with pixelation filter applied:</i>
<img src="/resources/images/preview_pix.png">


<h3>References</h3>
<b>Cel Shading:</b>
https://www.lighthouse3d.com/tutorials/glsl-12-tutorial/toon-shader-version-ii/

<b>Dithering: Taking inspiration from Return of the Obra Dinn</b><br>
https://www.alanzucconi.com/2018/10/24/shader-showcase-saturday-11/<br>
Adjust the bayer matrix through  https://www.shadertoy.com/view/WstXR8

<b>Fallen Particle:</b>
https://www.opengl-tutorial.org/intermediate-tutorials/billboards-particles/particles-instancing/

I use Chrono (https://cplusplus.com/reference/chrono/) to compute the time frame within the paintGL method.

<h3>Known Bugs:</h3>

Somehow I can’t run it in release mode and it crashes sometimes in the initial runs.

