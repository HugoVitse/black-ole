#version 430 core


layout (local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

// 2d image qu'on affiche
layout(rgba32f, binding = 0) uniform image2D imgOutput;

// milkyway equirectangular picture
uniform sampler2D skybox;
uniform vec2 viewAngles;
uniform float time;
uniform float fov;


#define M_PI 3.14159265358979323846
#define RS 2.0         // rayon de Schwarzschild
#define RMAX 100.0     // Distance max avant de stop


uniform vec3 camPos;


//disque d'accrétion

#define DISK_INNER 2.6 
#define DISK_OUTER 6.0 // Taille du disque
#define DISK_COLOR vec3(1.0, 0.8, 0.6) // Couleur de base (a voir si réaliste physiquement)


struct Photon {
    vec4 x; // r, theta, phi, t
    vec4 k; // kr, ktheta, kphi, kt
};

mat3 rotateY(float angle) { // Tourner gauche/droite (Yaw)
    float c = cos(angle); float s = sin(angle);
    return mat3(c, 0, s,  0, 1, 0,  -s, 0, c);
}
mat3 rotateX(float angle) { // Tourner haut/bas (Pitch)
    float c = cos(angle); float s = sin(angle);
    return mat3(1, 0, 0,  0, c, -s,  0, s, c);
}


//bruit simple pour le disque
float hash(vec2 p) {
    return fract(sin(dot(p, vec2(12.9898, 78.233))) * 43758.5453);
}

//new noise
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

float fbm(vec2 p) {
    float value = 0.0;
    float amp = 0.5; // Amplitude de départ
    float freq = 1.0; // Fréquence de départ
    
    // On fait 4 passes (Octaves)
    // Plus y'en a, plus c'est détaillé, mais plus c'est lourd pour la 1050 Ti
    for (int i = 0; i < 4; i++) {
        value += noise(p * freq) * amp;
        freq *= 2.1;  // On augmente la fréquence (détails plus fins)
        amp *= 0.5;   // On baisse l'amplitude (détails moins visibles)
        // Petit décalage pour éviter les symétries moches
        p += vec2(1.3, 2.4); 
    }
    return value;
}

// get color pour le disque
vec4 getDiskColor(float r, float phi, float k_phi) {
    // dégradé

    
    float rotationSpeed = 6.0 / pow(r, 2.5 );
    float movingPhi = phi - time * rotationSpeed;
    
    // --- TEXTURE DE NUAGES ---
    // Coordonnées de texture (UV)
    // Astuce : On multiplie phi par 3.0 mais r par 1.0
    // Ça "étire" les nuages en arc de cercle (effet traînée)
    vec2 cloudUV = vec2(r * 2.0, movingPhi * 3.0);
    
    // On génère le nuage (c'est ici que la magie opère)
    // On ajoute 'time' dans le 2ème paramètre pour que le gaz "bouillonne" un peu sur lui-même
    float cloud = fbm(cloudUV + vec2(time * 0.2, 0.0));
    
    // On contraste le nuage pour avoir des zones vides et des zones denses
    // (technique du "remapping")
    cloud = smoothstep(0.1, 0.9, cloud);

    // On ajoute des anneaux subtils par dessus
    float rings = sin(r * 15.0) * 0.2 + 0.8;
    float pattern = mix(cloud, cloud * rings, 0.3); // 70% nuage, 30% anneaux
    pattern = pattern * 0.9 + 0.1;

    // --- COULEUR & DOPPLER (Quasar Style) ---
    float intensity = 2.5 / (r - RS + 0.5); 
    float velocity = 3.5 / sqrt(r); 
    float shift = k_phi * velocity * 0.3; 

    // Palette Bleu/Blanc
    // vec3 colorHot  = vec3(0.9, 0.95, 1.0); 
    // vec3 colorBase = vec3(0.1, 0.4, 1.0); // Bleu profond
    // vec3 colorCold = vec3(0.1, 0.0, 0.2); // Violet sombre

    // Orange
    vec3 colorHot  = vec3(0.8, 0.9, 1.0);  // Blanc chaud (Blueshift max)
    vec3 colorBase = vec3(1.0, 0.55, 0.1); // Orange brûlé (Couleur réelle)
    vec3 colorCold = vec3(0.4, 0.05, 0.0); // Rouge sang (Redshift)

    vec3 col;
    if (shift < 0.0) { // Blueshift
        float factor = clamp(abs(shift) * 1.5, 0.0, 1.0);
        col = mix(colorBase, colorHot, factor);
        intensity *= (1.0 + factor * 2.5); 
    } else { // Redshift
        float factor = clamp(shift * 1.5, 0.0, 1.0);
        col = mix(colorBase, colorCold, factor);
        intensity *= (1.0 - factor * 0.7);
    }

    // On applique le motif nuageux
    col *= pattern;
    
    // Bords doux
    float edgeFade = smoothstep(DISK_OUTER, DISK_OUTER - 3.0, r);
    float alpha = clamp(intensity * pattern * edgeFade, 0.0, 0.98);

    return vec4(col * intensity, alpha);
}
//equivalent de la fonction F
// TODO renomer ca F dcp et implémenter le switch cristofel
Photon computeDerivatives(vec4 x, vec4 k) {
    float r = x.x;
    float th = x.y;
    
    // empêcher de diviser par 0 semble etre une bonne idée
    float s = sin(th);
    if (abs(s) < 1e-6) s = (s >= 0) ? 1e-6 : -1e-6;
    float cot_th = cos(th) / s;
    float r_2M = r - RS;
    if (abs(r_2M) < 1e-6) r_2M = 1e-6;

    // x devient le k préceédent
    float kr = k.x; float kth = k.y; float kph = k.z; float kt = k.t;
    float M = RS / 2.0;

    // nouvelles accélérations avec les symboles de cristoffel
    float dkr  = - ( (M*r_2M/(r*r*r))*kt*kt - (M/(r*r_2M))*kr*kr - r_2M*kth*kth - r_2M*s*s*kph*kph );
    float dkth = - ( (2.0/r)*kr*kth - s*cos(th)*kph*kph );
    float dkph = - ( (2.0/r)*kr*kph + 2.0*cot_th*kth*kph );
    float dkt  = - ( (2.0*M/(r*r_2M))*kr*kt );

    Photon p;
    p.x = vec4(kr, kth, kph, kt);    // dx/dt = k
    p.k = vec4(dkr, dkth, dkph, dkt); // dk/dt = accélération
    return p;
}


//cast photon
void main() {
    

    ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
    ivec2 dims = imageSize(imgOutput);

    // stop si hors de l'image
    if (pixel_coords.x >= dims.x || pixel_coords.y >= dims.y) return;


    float aspect_ratio = float(dims.x) / float(dims.y);
    
    // (-1 à 1)
    float u = (float(pixel_coords.x) / float(dims.x) - 0.5) * 2.0 * aspect_ratio;
    float v = (float(pixel_coords.y) / float(dims.y) - 0.5) * 2.0;

    //on ajoute la rotation de la souris
    vec3 rayDir = normalize(vec3(u*fov, v*fov, -1.0));
    rayDir = rotateY(viewAngles.x) * rotateX(viewAngles.y) * rayDir;

    vec4 x = vec4(camPos, 0.0);

   
    float fov = 1.0; 
    float k_theta = (v * fov) / x.x;
    float k_phi   = (u * fov) / (x.x * sin(x.y));

    // kr = -1.0 (Le photon part vers le trou noir)
    vec4 k = vec4(
        -rayDir.z, 
        -rayDir.y / x.x, 
         rayDir.x / (x.x * sin(x.y)), 
         0.0
    );


    // ensure initial photon's 4-momentum is null: g_{μν} k^μ k^ν = 0 (important)
    float f = 1.0 - RS/x.x;
    float spatial = (1.0/f)*k.x*k.x + x.x*x.x*k.y*k.y + x.x*x.x*sin(x.y)*sin(x.y)*k.z*k.z;
    k.w = sqrt(spatial / f);

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
        if (x.x <= RS * 1.01) {
            hitHorizon = true;
            break;
        }
        
        if (x.x > RMAX) {
            break;
        }

        // limite les sigularités aux poles
        if ((x.y < 0.01 && k.y < 0) || (x.y > M_PI - 0.01 && k.y > 0)) {
            k.y = -k.y;
            x.z += M_PI;
            x.y = (x.y < 1.0) ? 0.02 : M_PI - 0.02;
        }

        //RK4 steps
        Photon p = Photon(x, k);
        Photon k1 = computeDerivatives(p.x, p.k);
        
        Photon p2 = Photon(x + k1.x * (h*0.5), k + k1.k * (h*0.5));
        Photon k2 = computeDerivatives(p2.x, p2.k);
        
        Photon p3 = Photon(x + k2.x * (h*0.5), k + k2.k * (h*0.5));
        Photon k3 = computeDerivatives(p3.x, p3.k);
        
        Photon p4 = Photon(x + k3.x * h, k + k3.k * h);
        Photon k4 = computeDerivatives(p4.x, p4.k);

        x += (k1.x + k2.x*2.0 + k3.x*2.0 + k4.x) * (h/6.0);
        k += (k1.k + k2.k*2.0 + k3.k*2.0 + k4.k) * (h/6.0);

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