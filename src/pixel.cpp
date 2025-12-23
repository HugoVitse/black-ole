#include "pixel.hpp"
#include <cmath>
#include <stdio.h>
#include "skybox.hpp"

Pixel::Pixel(int _i, int _j, int W, int H, double h, Tetrade *tetrade, const Vec4 &camPos) : i(_i), j(_j) {

    double cam_r = camPos.x;
    double cam_theta = camPos.y;
    double cam_phi = camPos.z;

    double cx = cam_r * sin(cam_theta) * cos(cam_phi);
    double cy = cam_r * cos(cam_theta); // Y est l'axe vertical
    double cz = cam_r * sin(cam_theta) * sin(cam_phi);
    Vec3 pos = Vec3(cx, cy, cz);

    // 2. Calcul des vecteurs de vue (LookAt)
    Vec3 forward = (Vec3(0,0,0) - pos).normalize();
    Vec3 worldUp = Vec3(0, 1, 0); 
    Vec3 right = forward.cross(worldUp).normalize();
    Vec3 up = right.cross(forward).normalize();

    // 3. Direction du rayon (xtmp, ytmp, ztmp)
    double w = h * (double)W / (double)H;
    double xtmp = ((2.0 * w) / W) * (i - W / 2.0);
    double ytmp = ((2.0 * h) / H) * (j - H / 2.0);
    double ztmp = 1.0; 

    Vec3 dir = (forward * ztmp + right * xtmp + up * ytmp).normalize();

    this->photon.state.x = Vec4(pos.x, pos.y, pos.z, 0.0);
    this->photon.state.k = Vec4(dir.x, dir.y, dir.z, 1.0);
}


Pixel::Pixel() : r(0), g(0), b(0), i(0), j(0), k_local(0,0,0,0), k_global(0,0,0,0), photon(Vec4(0,0,0,0), Vec4(0,0,0,0)) {}



void Pixel::castPhoton(const BlackHole &blackhole){
    // 1. Initialisation de la condition de moment nul (g_uv k^u k^v = 0)
    {
        Vec4 &pos = this->photon.state.x; // (x, y, z, t)
        Vec4 &k = this->photon.state.k;   // (kx, ky, kz, kt)

        double r2 = pos.x*pos.x + pos.y*pos.y + pos.z*pos.z;
        double r = sqrt(r2);
        double M = blackhole.mass;
        double f = 1.0 - (2.0 * M / r);

        if (r > 2.0 * M) {
            // Produit scalaire (Position . K_spatial)
            double x_dot_k = pos.x * k.x + pos.y * k.y + pos.z * k.z;
            
            // En cartésien Schwarzschild, le produit scalaire spatial g_ij k^i k^j est :
            // (k.k) + (2M / (r^2 * (r - 2M))) * (pos.k)^2
            double k_dot_k = k.x*k.x + k.y*k.y + k.z*k.z;
            double spatial_part = k_dot_k + (2.0 * M / (r2 * (r - 2.0 * M))) * (x_dot_k * x_dot_k);

            // On résout g_tt kt^2 + spatial_part = 0  => -f*kt^2 + spatial_part = 0
            double kt = sqrt(spatial_part / f);

            // Inversion pour le backward raytracing
            k.x = -k.x;  
            k.y = -k.y;  
            k.z = -k.z;  
            k.t = kt;    
        }
    }

    const int maxIter = 2000;
    int iter = 0;
    double base_step = 1.0;
    double min_step = 0.01;
    double max_step = 10.0;
    
    // 2. Boucle d'intégration
    // On calcule r à chaque fois avec Pythagore
    double current_r = this->photon.state.x.norm(); 

    while(current_r > blackhole.rs && current_r < blackhole.rmax) {
        double r_before = current_r; // Utilise current_r, pas r

        double step_size;
        // On utilise current_r pour le pas adaptatif
        if (current_r < 10.0 * blackhole.rs) {
            step_size = min_step + (base_step - min_step) * (current_r - blackhole.rs) / (9.0 * blackhole.rs);
        } else if (current_r < 100.0 * blackhole.rs) {
            step_size = base_step;
        } else {
            step_size = std::min(max_step, base_step * (current_r / (100.0 * blackhole.rs)));
        }
        
        this->photon.RK4step(blackhole, -step_size);

        // Mise à jour de la distance
        current_r = this->photon.state.x.norm();

        if (current_r > r_before * 2.0 && r_before < 4.0 * blackhole.rs) {
            this->photon.state.x = Vec4(0,0,0,0); // On force le noir
            break; 
        }

        if (std::isnan(current_r)) {
            if (r_before < 1.5 * blackhole.rs) {
                this->photon.state.x.x = 0.0; // Signal Trou noir
                this->photon.state.x.y = 0.0;
                this->photon.state.x.z = 0.0;
            }
            break; 
        }
        
        // Cheese Naan (NaN protection globale)
        bool anyNan = std::isnan(this->photon.state.x.x) || std::isnan(this->photon.state.x.y)
                   || std::isnan(this->photon.state.x.z) || std::isnan(this->photon.state.k.x);

        if(anyNan) break;
        
        if(++iter > maxIter) break;
    }
}

void Pixel::setColor(const BlackHole &blackhole, const Skybox &skybox) {
    // 1. Calculer la distance réelle (norme)
    double x = this->photon.state.x.x;
    double y = this->photon.state.x.y;
    double z = this->photon.state.x.z;
    double r_final = sqrt(x*x + y*y + z*z);

    // 2. NOIR : Si le photon est sous l'horizon (Rayon < rs)
    if (r_final <= blackhole.rs * 1.001 || std::isnan(r_final)) {
        this->r = 0; this->g = 0; this->b = 0;
        return;
    }

    // 3. SKYBOX : Pour tout le reste
    // On convertit la direction finale en angles pour la texture
    double theta = acos(y / r_final); 
    double phi = atan2(z, x);

    int resR, resG, resB;
    skybox.getColor(theta, phi, resR, resG, resB);
    
    this->r = resR; this->g = resG; this->b = resB;
}