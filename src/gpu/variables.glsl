
// threads sur gpu
layout (local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

// 2d image qu'on affiche
layout(rgba32f, binding = 0) uniform image2D imgOutput;

// uniforms variables envoyées depuis C++
uniform sampler2D skybox; // texture skybox
uniform sampler2D sunTex; // texture soleil

uniform vec2 viewAngles; //camera
uniform float time; 
uniform float fov;

uniform vec3 camPos;

uniform float sunRadius;

#define M_PI 3.14159265358979323846

BlackHole blackhole = BlackHole(1.0,2.0,100.00,0.0,0.0);
Tetrade tetrade = Tetrade( vec4(0,0,0,0) , vec4(0,0,0,0) ,vec4(0,0,0,0),vec4(0,0,0,0));
Pixel pixel = Pixel(vec3(0,0,0),0,0, Photon(vec4(0,0,0,0),vec4(0,0,0,0)));
AccretionDisk disk = AccretionDisk(2.6,6.0,vec3(2.0, 0.8, 0.6));


int maxIter = 2000;  // Nombre de pas max
float pas = 0.002; 



vec4 finalColor = vec4(0.0);
float transmittance = 1.0;
bool hitHorizon = false;

