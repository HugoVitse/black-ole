#include <stdio.h>
#include <iostream>
#include "vectors.hpp"
#include "blackhole.hpp"
#include "image.hpp"


int main(int argc, char** argv) {

   
    const double PI = 3.14;
    const int nb_frames = 120; // 5 secondes à 24 fps

    BlackHole* bh = new BlackHole(1);

    const double tour = PI/6;
 

    for (int f = 0; f < nb_frames; f++) {
        double angle = (tour * f) / nb_frames; //faire bouger l'angle de camera pour faire genre le ciel bouge

        Vec4 camPos(20.0, PI/2, angle, 0);
        
        // résolution
        const int W = 800;
        const int H = 450;
        double FOV = PI/3; 
        
        Image* image = new Image(W, H, FOV, camPos);
        
        char filename[64];
        sprintf(filename, "output/frame_%03d.ppm", f);
        image->computeImage(*bh, camPos);
        image->saveImage(filename);
        

        delete image;
        
    }
    
    delete bh;
    

}