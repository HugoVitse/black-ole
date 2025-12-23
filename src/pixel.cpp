#include "pixel.hpp"
#include <cmath>
#include <stdio.h>
#include "skybox.hpp"

Pixel::Pixel(int _i, int _j, int W, int H, double h, Tetrade *tetrade, const Vec4 &camPos) : i(_i), j(_j) {

    //ratio conservé
    double w = h * W/H;

    //on passe à l'interval -w; w
    double xtmp = ((2.0 * w) / double(W)) * (double(i) - (double(W) / 2.0));
    double ytmp = ((2.0 * h) / double(H)) * (double(j) - (double(H) / 2.0));
    double ztmp = 1;
    
    

    //normalisation
    double norm = sqrt( (xtmp*xtmp) + (ytmp*ytmp) + (ztmp*ztmp) );


    this->k_local = Vec4( -ztmp/norm, ytmp/norm, xtmp/norm, 1 );
   

    //passage au référentiel global (changement de base)
    Vec4 e0 = *tetrade->e0;
    Vec4 e1 = *tetrade->e1;
    Vec4 e2 = *tetrade->e2;
    Vec4 e3 = *tetrade->e3;

     

    this->k_global =  Vec4(
        this->k_local * e0,
        this->k_local * e1,
        this->k_local * e2,
        this->k_local * e3
    );

 
    //brigad eanti divergence
    this->photon = Photon(camPos, this->k_global);
    if (std::abs(std::sin(camPos.theta)) < 1e-5) {
        this->photon.state.x.y += 1e-6; 
    }
    if (this->photon.state.x.theta < 0.001) this->photon.state.x.theta = 0.001;
    if (this->photon.state.x.theta > M_PI - 0.001) this->photon.state.x.theta = M_PI - 0.001;
}


Pixel::Pixel() : r(0), g(0), b(0), i(0), j(0), k_local(0,0,0,0), k_global(0,0,0,0), photon(Vec4(0,0,0,0), Vec4(0,0,0,0)) {}



void Pixel::castPhoton(const BlackHole &blackhole){
    // ensure initial photon's 4-momentum is null: g_{μν} k^μ k^ν = 0 (important)
    {
        const Vec4 &x = this->photon.state.x; // initial position (camera)
        Vec4 &k = this->photon.state.k;

        double r = x.r;
        double th = x.theta;
        double twoM = 2.0 * blackhole.mass;
        double f = 1.0 - twoM / r; 


        if (r > twoM && std::abs(f) > 1e-16) {
            double g_tt = -f;
            double g_rr = 1.0 / f;
            double g_thth = r * r;
            double g_phph = r * r * sin(th) * sin(th);

            double spatial = g_rr * (k.x * k.x) + g_thth * (k.y * k.y) + g_phph * (k.z * k.z);

            if (spatial >= 0 && -g_tt > 0) {
                double kt = sqrt(spatial / (-g_tt));
                //on inverse k car on est en backward raytracing (sauf temp)
                k.x = -k.x;  
                k.y = -k.y;  
                k.z = -k.z;  
                k.t = kt;    
            } else {
                // je met rien ici je men fou
            }
        }
    }

    //si le photon se pomme (il peut flotter pendant 1000 ans) on le stop au bout de 2000
    const int maxIter = 2000;
    int iter = 0;
    
    // pour accélérer le rendu, on modifie le step en fonction de la distance 
    // plus on est loin du trou noir plus le comportement du bordel est régulier => pas obligé d'avoir un pas très précis
    double base_step = 1;
    double min_step = 0.1;
    double max_step = 10.0;
    
    // debug
    if(this->i == 0 && this->j == 0) {
        printf("Pixel (0,0): r_init=%.2f, k_r=%.6f, k_theta=%.6f, k_phi=%.6f, k_t=%.6f\n",
               this->photon.state.x.x, 
               this->photon.state.k.x, this->photon.state.k.y, 
               this->photon.state.k.z, this->photon.state.k.t);
    }
    
    while(this->photon.state.x.x > blackhole.rs && this->photon.state.x.x < blackhole.rmax) {
        double r = this->photon.state.x.x;
        double r_before = r;

        //brigade anti divergence
     
            

        double step_size;
        if (r < 10.0 * blackhole.rs) {

            step_size = min_step + (base_step - min_step) * (r - blackhole.rs) / (9.0 * blackhole.rs);
            step_size = std::max(min_step, std::min(step_size, base_step));

        } else if (r < 100.0 * blackhole.rs) {
            step_size = base_step;
        } else {
            step_size = std::min(max_step, base_step * (r / (100.0 * blackhole.rs)));
        }
        
        this->photon.RK4step(blackhole, -step_size);

        //brigae antidivegence
        if (this->photon.state.x.y < 0.0001) this->photon.state.x.y = 0.0001;
        if (this->photon.state.x.y > M_PI - 0.0001) this->photon.state.x.y = M_PI - 0.0001;

        if (std::abs(this->photon.state.k.r) > 1e4|| std::isnan(this->photon.state.x.x)) { 
            if (r_before < 1.5 * blackhole.rs) {
                this->photon.state.x.x = 0.0; 
            }
            break; 
        }
        
        // debug
        if(this->i == 0 && this->j == 0 && iter < 10) {
            double r_after = this->photon.state.x.x;
            printf("  iter %d: r %.2f -> %.2f (dr=%.2f), k_r=%.6f\n", 
                   iter, r_before, r_after, r_after - r_before, this->photon.state.k.x);
        }
        
        // cheese naan
        bool anyNan = std::isnan(this->photon.state.x.x) || std::isnan(this->photon.state.x.y)
                   || std::isnan(this->photon.state.x.z) || std::isnan(this->photon.state.x.t)
                   || std::isnan(this->photon.state.k.x) || std::isnan(this->photon.state.k.y)
                   || std::isnan(this->photon.state.k.z) || std::isnan(this->photon.state.k.t);

        if(anyNan) {
            // stop integration;
            break;
        }
        
        if(++iter > maxIter) {
            // safety stop
            this->r= NAN;
            break;
        }
    }

}

void Pixel::setColor(const BlackHole &blackhole, const Skybox &skybox){

    double multiple = 50.0;
    // si chicken naban ou sous l'horizon -> Noir
    if (this->photon.state.x.x <= blackhole.rs) {
        this->r = 255; this->g = 0; this->b = 0;
        return;
    }

   

    double theta_final = this->photon.state.x.y;
    double phi_final   = this->photon.state.x.z;

    int resR, resG, resB;
    skybox.getColor(theta_final, phi_final, resR, resG, resB);
    
    this->r = resR;
    this->g = resG;
    this->b = resB;

    return;
    //old one, starmap automaticaly generated
    // double star_value = sin(theta_final * multiple) * cos(phi_final * multiple);

    // if (star_value > 0.995) { 
    //     this->r = 255; this->g = 255; this->b = 255; // Étoile
    // } else {
    //     this->r = 255; this->g = 0; this->b = 0; 
    // }
    // return;
    


}


