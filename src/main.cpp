#include <stdio.h>
#include <iostream>
#include "vectors.hpp"
#include "blackhole.hpp"
#include "image.hpp"


int main(int argc, char** argv) {

   
    const double PI = 3.14;
    const int nb_frames = 120; // 5 secondes à 24 fps

    BlackHole* bh = new BlackHole(1);
    Skybox* sb = new Skybox();
    sb->load("assets/skybox2.ppm");


    const double tour = PI/12;
 

    for (int f = 0; f < nb_frames; f++) {
        double angle = (tour * f) / nb_frames; //faire bouger l'angle de camera pour faire genre le ciel bouge

        Vec4 camPos(10.0, PI/2, angle, 0);
        
        // résolution
        const int W = 8000;
        const int H = 4000;
        double FOV = PI/3; 
        
        Image* image = new Image(W, H, FOV, camPos);
        
        char filename[64];
        sprintf(filename, "output/frame_%03d.ppm", f);
        image->computeImage(*bh, *sb, camPos);
        image->saveImage(filename);
        

        delete image;
        
    }
    
    delete bh;
    

}