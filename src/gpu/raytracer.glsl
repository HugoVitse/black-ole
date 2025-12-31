// threads sur gpu
layout (local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

// 2d image qu'on affiche
layout(rgba32f, binding = 0) uniform image2D imgOutput;

// uniforms variables envoyées depuis C++
uniform sampler2D skybox; // texture skybox
uniform vec2 viewAngles; //camera
uniform float time; 
uniform float fov;
uniform vec3 camPos;


//constantes

#define M_PI 3.14159265358979323846

//disque d'accrétion
#define DISK_INNER 2.6 
#define DISK_OUTER 6.0 // Taille du disque
#define DISK_COLOR vec3(2.0, 0.8, 0.6) // Couleur de base (a voir si réaliste physiquement)



BlackHole blackhole = BlackHole(1.0,2.0,100.00);


// //iq noise for accretion disc
// float noise(vec2 p) {
//     vec2 i = floor(p);
//     vec2 f = fract(p);
    
//     vec2 u = f * f * (3.0 - 2.0 * f);
    
//     float a = fract(sin(dot(i + vec2(0.0, 0.0), vec2(12.9898, 78.233))) * 43758.5453);
//     float b = fract(sin(dot(i + vec2(1.0, 0.0), vec2(12.9898, 78.233))) * 43758.5453);
//     float c = fract(sin(dot(i + vec2(0.0, 1.0), vec2(12.9898, 78.233))) * 43758.5453);
//     float d = fract(sin(dot(i + vec2(1.0, 1.0), vec2(12.9898, 78.233))) * 43758.5453);

//     return mix(mix(a, b, u.x), mix(c, d, u.x), u.y);
// }

// //mouvement brownien
// // iq noise and fbm are inspired by the following  https://iquilezles.org/articles/morenoise/
// float fbm(vec2 p) {
//     float value = 0.0;
//     float amp = 0.5; 
//     float freq = 1.0; 
    
//     for (int i = 0; i < 8; i++) {
//         value += noise(p * freq) * amp;
//         freq *= 2.1;  
//         amp *= 0.5;   
//         p += vec2(1.3, 2.4); 
//     }
//     return value;
// }

// // get color pour le disque
// vec4 getDiskColor(float r, float phi, float k_phi) {

//     float rotationSpeed = 6.0 / pow(r, 2.5 );
//     float movingPhi = phi - time * rotationSpeed;
 
//     vec2 cloudUV = vec2(r * 2.0, movingPhi * 3.0);
//     float cloud = fbm(cloudUV + vec2(time * 0.2, 0.0));

//     cloud = smoothstep(0.1, 0.9, cloud);


//     float rings = sin(r * 15.0) * 0.2 + 0.8;
//     float pattern = mix(cloud, cloud * rings, 0.3); // 70% nuage, 30% anneaux
//     pattern = pattern * 0.9 + 0.1;

 
//     float intensity = 2.5 / (r - blackhole.rs + 0.5); 
//     float velocity = 3.5 / sqrt(r); 
//     float shift = k_phi * velocity * 0.3; 

//     // bleu -> chaud
//     // vec3 colorHot  = vec3(0.9, 0.95, 1.0); 
//     // vec3 colorBase = vec3(0.1, 0.4, 1.0); 
//     // vec3 colorCold = vec3(0.1, 0.0, 0.2); 

//     // oeange moins réaliste
//     vec3 colorHot  = vec3(0.8, 0.9, 1.0); 
//     vec3 colorBase = vec3(1.0, 0.55, 0.1); 
//     vec3 colorCold = vec3(0.4, 0.05, 0.0);

//     vec3 col;


//     //doppler effect
//     if (shift < 0.0) { // blueshift
//         float factor = clamp(abs(shift) * 1.5, 0.0, 1.0);
//         col = mix(colorBase, colorHot, factor);
//         intensity *= (1.0 + factor * 2.5); 
//     } else { // redshift
//         float factor = clamp(shift * 1.5, 0.0, 1.0);
//         col = mix(colorBase, colorCold, factor);
//         intensity *= (1.0 - factor * 0.7);
//     }

//     col *= pattern;

//     float edgeFade = smoothstep(DISK_OUTER, DISK_OUTER - 3.0, r);
//     float alpha = clamp(intensity * pattern * edgeFade, 0.0, 0.98);

//     return vec4(col * intensity, alpha);
// }




//cast photon
void main() {
    

    ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
    float _h = 1/tan(fov);
   

    Tetrade tetrade = Tetrade( vec4(0,0,0,0) , vec4(0,0,0,0) ,vec4(0,0,0,0),vec4(0,0,0,0));
    initTetrade(tetrade, vec4(camPos,0.0) );

    Pixel pixel = Pixel(vec3(0,0,0),0,0, Photon(vec4(0,0,0,0),vec4(0,0,0,0)));
    initPixel(pixel, _h ,camPos, imgOutput, viewAngles, fov, tetrade);



    // classic RK4
    int maxIter = 600;  // Nombre de pas max
    float h = 0.02;      // augmente ca et la bande noire revient

    vec4 finalColor = vec4(0.0);
    float transmittance = 1.0;

    bool hitHorizon = false;


    AccretionDisk disk = AccretionDisk(2.6,6.0,vec3(2.0, 0.8, 0.6));

    for(int i = 0; i < maxIter; i++) {

        float prevTheta = pixel.photon.x.y;  //sauvegarde des coordonnées précédentes pour detecter une potnetielle traversée de disque
        float prevR = pixel.photon.x.x;

    

        //RK4 steps
        
        RK4step(blackhole, h, pixel.photon);
        // stop conditions
        if (pixel.photon.x.x <= blackhole.rs * 1.01) {
            hitHorizon = true;
            break;
        }
        
        if (pixel.photon.x.x > blackhole.rmax) {
            break;
        }


        if(!handleCollision(prevTheta, prevR, pixel, disk, finalColor, transmittance, time, blackhole)) break;
        // //detection de traversage de disque
        // if ((prevTheta - M_PI/2.0) * (pixel.photon.x.y - M_PI/2.0) < 0.0) {

           
        //     float factor = abs(prevTheta - M_PI/2.0) / abs(pixel.photon.x.y - prevTheta);
        //     float r_cross = mix(prevR, pixel.photon.x.x, factor);

        //     if (r_cross > DISK_INNER && r_cross < DISK_OUTER) {
        //         vec4 diskCol = getDiskColor(r_cross, pixel.photon.x.z, pixel.photon.k.z);
                
            
        //         finalColor += diskCol * transmittance * diskCol.a;
        //         transmittance *= (1.0 - diskCol.a);
                
        //         if (transmittance < 0.05) break;
        //     }
        // }
        


    }

    // set color
   
    setColor(hitHorizon, transmittance, finalColor ,skybox, pixel);
    imageStore(imgOutput, pixel_coords, vec4(pixel.color, 1.0));
}
