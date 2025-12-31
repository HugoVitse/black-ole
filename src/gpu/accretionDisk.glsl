

struct AccretionDisk {

    float disk_inner;
    float disk_outer;// Taille du disque
    vec3 disk_color;// Couleur de base (a voir si réaliste physiquement)

};



// iq noise and fbm are inspired by the following  https://iquilezles.org/articles/morenoise/
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
vec4 getDiskColor(inout float r, inout float phi, inout float k_phi, inout float time, inout BlackHole blackhole, inout AccretionDisk disk) {

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

    float edgeFade = smoothstep(disk.disk_outer, disk.disk_outer - 3.0, r);
    float alpha = clamp(intensity * pattern * edgeFade, 0.0, 0.98);

    return vec4(col * intensity, alpha);
}


bool handleCollision(inout float prevTheta, inout float prevR , inout Pixel pixel, inout AccretionDisk disk, inout vec4 finalColor, inout float transmittance, float time , BlackHole blackhole) {

    if ((prevTheta - M_PI/2.0) * (pixel.photon.x.y - M_PI/2.0) < 0.0) {

        
        float factor = abs(prevTheta - M_PI/2.0) / abs(pixel.photon.x.y - prevTheta);
        float r_cross = mix(prevR, pixel.photon.x.x, factor);

        if (r_cross > disk.disk_inner && r_cross < disk.disk_outer) {
            vec4 diskCol = getDiskColor(r_cross, pixel.photon.x.z, pixel.photon.k.z, time, blackhole, disk);
            
        
            finalColor += diskCol * transmittance * diskCol.a;
            transmittance *= (1.0 - diskCol.a);
            
            if (transmittance < 0.05) return false;
            else return true;
        }
    }
    return true;
}