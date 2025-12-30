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



// camera rotations
mat3 rotateY(float angle) { 
    float c = cos(angle); float s = sin(angle);
    return mat3(c, 0, s,  0, 1, 0,  -s, 0, c);
}
mat3 rotateX(float angle) { 
    float c = cos(angle); float s = sin(angle);
    return mat3(1, 0, 0,  0, c, -s,  0, s, c);
}




//iq noise for accretion disc
float noise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    
    vec2 u = f * f * (3.0 - 2.0 * f);
    
    float a = fract(sin(dot(i + vec2(0.0, 0.0), vec2(12.9898, 78.233))) * 43758.5453);
    float b = fract(sin(dot(i + vec2(1.0, 0.0), vec2(12.9898, 78.233))) * 43758.5453);
    float c = fract(sin(dot(i + vec2(0.0, 1.0), vec2(12.9898, 78.233))) * 43758.5453);
    float d = fract(sin(dot(i + vec2(1.0, 1.0), vec2(12.9898, 78.233))) * 43758.5453);

    return mix(mix(a, b, u.x), mix(c, d, u.x), u.y);
}

//mouvement brownien
// iq noise and fbm are inspired by the following  https://iquilezles.org/articles/morenoise/
float fbm(vec2 p) {
    float value = 0.0;
    float amp = 0.5; 
    float freq = 1.0; 
    
    for (int i = 0; i < 8; i++) {
        value += noise(p * freq) * amp;
        freq *= 2.1;  
        amp *= 0.5;   
        p += vec2(1.3, 2.4); 
    }
    return value;
}

// get color pour le disque
vec4 getDiskColor(float r, float phi, float k_phi) {

    float rotationSpeed = 6.0 / pow(r, 2.5 );
    float movingPhi = phi - time * rotationSpeed;
 
    vec2 cloudUV = vec2(r * 2.0, movingPhi * 3.0);
    float cloud = fbm(cloudUV + vec2(time * 0.2, 0.0));

    cloud = smoothstep(0.1, 0.9, cloud);


    float rings = sin(r * 15.0) * 0.2 + 0.8;
    float pattern = mix(cloud, cloud * rings, 0.3); // 70% nuage, 30% anneaux
    pattern = pattern * 0.9 + 0.1;

 
    float intensity = 2.5 / (r - blackhole.rs + 0.5); 
    float velocity = 3.5 / sqrt(r); 
    float shift = k_phi * velocity * 0.3; 

    // bleu -> chaud
    // vec3 colorHot  = vec3(0.9, 0.95, 1.0); 
    // vec3 colorBase = vec3(0.1, 0.4, 1.0); 
    // vec3 colorCold = vec3(0.1, 0.0, 0.2); 

    // oeange moins réaliste
    vec3 colorHot  = vec3(0.8, 0.9, 1.0); 
    vec3 colorBase = vec3(1.0, 0.55, 0.1); 
    vec3 colorCold = vec3(0.4, 0.05, 0.0);

    vec3 col;


    //doppler effect
    if (shift < 0.0) { // blueshift
        float factor = clamp(abs(shift) * 1.5, 0.0, 1.0);
        col = mix(colorBase, colorHot, factor);
        intensity *= (1.0 + factor * 2.5); 
    } else { // redshift
        float factor = clamp(shift * 1.5, 0.0, 1.0);
        col = mix(colorBase, colorCold, factor);
        intensity *= (1.0 - factor * 0.7);
    }

    col *= pattern;

    float edgeFade = smoothstep(DISK_OUTER, DISK_OUTER - 3.0, r);
    float alpha = clamp(intensity * pattern * edgeFade, 0.0, 0.98);

    return vec4(col * intensity, alpha);
}




//cast photon
void main() {
    

    ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
    ivec2 dims = imageSize(imgOutput);

    float aspect_ratio = float(dims.x) / float(dims.y);
    
    // (-1 à 1)
    float u = (float(pixel_coords.x) / float(dims.x) - 0.5) * 2.0 * aspect_ratio;
    float v = (float(pixel_coords.y) / float(dims.y) - 0.5) * 2.0;

    //on ajoute la rotation de la souris
    vec3 rayDir = normalize(vec3(u*fov, v*fov, -1.0));
    rayDir = rotateY(viewAngles.x) * rotateX(viewAngles.y) * rayDir;

    vec4 x = vec4(camPos, 0.0);

   



    // kr = -1.0 (Le photon part vers le trou noir)
    vec4 k = vec4(
        -rayDir.z, 
        -rayDir.y / x.x, 
         rayDir.x / (x.x * sin(x.y)), 
         0.0
    );


    // ensure initial photon's 4-momentum is null: g_{μν} k^μ k^ν = 0 (important)
    float f = 1.0 - blackhole.rs/x.x;
    float spatial = (1.0/f)*k.x*k.x + x.x*x.x*k.y*k.y + x.x*x.x*sin(x.y)*sin(x.y)*k.z*k.z;
    k.w = sqrt(spatial / f);

    // Photon photon = Photon(x,k);

    // classic RK4
    int maxIter = 600;  // Nombre de pas max
    float h = 0.02;      // augmente ca et la bande noire revient

    vec4 finalColor = vec4(0.0);
    float transmittance = 1.0;

    bool hitHorizon = false;

    for(int i = 0; i < maxIter; i++) {

        float prevTheta = x.y;  //sauvegarde des coordonnées précédentes pour detecter une potnetielle traversée de disque
        float prevR = x.x;

        // stop conditions
        if (x.x <= blackhole.rs * 1.01) {
            hitHorizon = true;
            break;
        }
        
        if (x.x > blackhole.rmax) {
            break;
        }


        //RK4 steps
        Photon photon = Photon(x,k);
        RK4step(blackhole, h, photon);

        x = photon.x;
        k = photon.k;

        //detection de traversage de disque
        if ((prevTheta - M_PI/2.0) * (x.y - M_PI/2.0) < 0.0) {

           
            float factor = abs(prevTheta - M_PI/2.0) / abs(x.y - prevTheta);
            float r_cross = mix(prevR, x.x, factor);

            if (r_cross > DISK_INNER && r_cross < DISK_OUTER) {
                vec4 diskCol = getDiskColor(r_cross, x.z, k.z);
                
            
                finalColor += diskCol * transmittance * diskCol.a;
                transmittance *= (1.0 - diskCol.a);
                
                if (transmittance < 0.05) break;
            }
        }
    }

    // set color
    vec3 color;

    if (hitHorizon) {
        color = vec3(0.0); // noir
    } else {
        // convertisseur sphérique vers uv de la skybox
        float u_tex = x.z / (2.0 * M_PI);
        float v_tex = x.y / M_PI;
        
        color = texture(skybox, vec2(u_tex, v_tex)).rgb;
    }

    vec3 result = finalColor.rgb + color * transmittance;
    imageStore(imgOutput, pixel_coords, vec4(result, 1.0));
}
