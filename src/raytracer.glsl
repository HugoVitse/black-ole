#version 430 core

// On travaille par groupes de 8x8 pixels
layout (local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

// L'image finale qu'on affiche à l'écran
layout(rgba32f, binding = 0) uniform image2D imgOutput;

// La texture de la Voie Lactée (envoyée par le C++)
uniform sampler2D skybox;

// --- CONSTANTES PHYSIQUES ---
#define M_PI 3.14159265358979323846
#define RS 2.0         // Rayon de Schwarzschild (Horizon des événements)
#define RMAX 100.0     // Distance max avant d'arrêter le calcul (Ciel)

// Position de la caméra (r=15.0, theta=PI/2, phi=0.0)
uniform vec3 camPos;
// Structure pour stocker Position (x) et Vitesse (k)
struct Photon {
    vec4 x; // r, theta, phi, t
    vec4 k; // kr, ktheta, kphi, kt
};

// --- CŒUR DU CALCUL : Les équations d'Einstein (Christoffel) ---
Photon computeDerivatives(vec4 x, vec4 k) {
    float r = x.x;
    float th = x.y;
    
    // Sécurités mathématiques (éviter division par zéro)
    float s = sin(th);
    if (abs(s) < 1e-6) s = (s >= 0) ? 1e-6 : -1e-6;
    float cot_th = cos(th) / s;
    float r_2M = r - RS;
    if (abs(r_2M) < 1e-6) r_2M = 1e-6;

    // Récupération des vitesses actuelles
    float kr = k.x; float kth = k.y; float kph = k.z; float kt = k.t;
    float M = RS / 2.0;

    // Calcul des accélérations (Géodésiques de Schwarzschild)
    float dkr  = - ( (M*r_2M/(r*r*r))*kt*kt - (M/(r*r_2M))*kr*kr - r_2M*kth*kth - r_2M*s*s*kph*kph );
    float dkth = - ( (2.0/r)*kr*kth - s*cos(th)*kph*kph );
    float dkph = - ( (2.0/r)*kr*kph + 2.0*cot_th*kth*kph );
    float dkt  = - ( (2.0*M/(r*r_2M))*kr*kt );

    Photon p;
    p.x = vec4(kr, kth, kph, kt);    // dx/dt = k
    p.k = vec4(dkr, dkth, dkph, dkt); // dk/dt = accélération
    return p;
}

void main() {
    // Coordonnées du pixel
    ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
    ivec2 dims = imageSize(imgOutput);

    // Arrêt si hors de l'image
    if (pixel_coords.x >= dims.x || pixel_coords.y >= dims.y) return;

    // --- 1. GÉNÉRATION DU RAYON ---
    float aspect_ratio = float(dims.x) / float(dims.y);
    
    // Coordonnées normalisées (-1 à 1)
    float u = (float(pixel_coords.x) / float(dims.x) - 0.5) * 2.0 * aspect_ratio;
    float v = (float(pixel_coords.y) / float(dims.y) - 0.5) * 2.0;

    vec4 x = vec4(camPos, 0.0);

    // VECTEUR VITESSE INITIAL
    // Correction FOV : On divise par la distance (x.x) pour viser correctement le centre
    float fov = 1.0; 
    float k_theta = (v * fov) / x.x;
    float k_phi   = (u * fov) / (x.x * sin(x.y));

    // kr = -1.0 (Le photon part vers le trou noir)
    vec4 k = vec4(-1.0, k_theta, k_phi, 0.0);

    // Calcul de kt pour que ce soit un photon (norme = 0)
    float f = 1.0 - RS/x.x;
    float spatial = (1.0/f)*k.x*k.x + x.x*x.x*k.y*k.y + x.x*x.x*sin(x.y)*sin(x.y)*k.z*k.z;
    k.w = sqrt(spatial / f);

    // --- 2. BOUCLE DE RENDU (RK4) ---
    int maxIter = 600;  // Nombre de pas max
    float h = 0.2;      // Taille du pas (plus petit = plus précis)

    bool hitHorizon = false;

    for(int i = 0; i < maxIter; i++) {
        // Condition de sortie : Trou noir touché
        if (x.x <= RS * 1.01) {
            hitHorizon = true;
            break;
        }
        // Condition de sortie : Parti dans le ciel
        if (x.x > RMAX) {
            break;
        }

        // Saut de pôle (Math hack pour éviter la singularité en theta=0 ou PI)
        if ((x.y < 0.02 && k.y < 0) || (x.y > M_PI - 0.02 && k.y > 0)) {
            k.y = -k.y;
            x.z += M_PI;
            x.y = (x.y < 1.0) ? 0.02 : M_PI - 0.02;
        }

        // Runge-Kutta 4
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
    }

    // --- 3. COULEUR FINALE ---
    vec3 color;

    if (hitHorizon) {
        color = vec3(0.0); // NOIR PARFAIT
    } else {
        // On va chercher la couleur dans la texture Skybox
        // Mapping sphérique (u=phi, v=theta)
        float u_tex = x.z / (2.0 * M_PI);
        float v_tex = x.y / M_PI;
        
        color = texture(skybox, vec2(u_tex, v_tex)).rgb;
    }

    // Écriture dans l'image
    imageStore(imgOutput, pixel_coords, vec4(color, 1.0));
}