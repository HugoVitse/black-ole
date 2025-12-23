#include "image.hpp"
#include <math.h>
#include <fstream>
#include <cmath>
#include <thread>
#include <functional>
#include <vector>
#include <algorithm>
#include <iostream>
#include <mutex>

// nombre de threads 
int Image::nb_threads = 2*std::max(1u, std::thread::hardware_concurrency());

// mutex pour les threads
static std::mutex progress_mutex;

Image::Image(int _W, int _H, double FOV, const Vec4 &camPos) : W(_W), H(_H), tetrade(camPos) {
    this->pixels.resize(W * H);
    this->h = 1.0 / tan(FOV / 2.0);
}



void Image::computeRow(const BlackHole &blackhole, const Skybox &skybox, int start_y, int end_y, const Vec4 &camPos) {
    for(int j = start_y; j < end_y; j++) {
        for(int i = 0; i < this->W; i++) {
            int index = j * W + i;
            pixels[index] = Pixel(i, j, W, H, h, &tetrade, camPos);
            pixels[index].castPhoton(blackhole);
            pixels[index].setColor(blackhole, skybox);
        }
    }
}

void Image::computeImage(const BlackHole &blackhole, const Skybox &skybox, const Vec4 &camPos) {
    std::cout << "Rendu en cours avec " << nb_threads << " threads..." << std::endl;
    
    threads.clear();
    int rows_per_thread = H / nb_threads;

    for(int i = 0; i < nb_threads; i++) {
        int start_y = i * rows_per_thread;
        int end_y = (i == nb_threads - 1) ? H : (i + 1) * rows_per_thread;

        threads.push_back(std::thread(
            &Image::computeRow, 
            this, 
            std::cref(blackhole), 
            std::cref(skybox),
            start_y, 
            end_y, 
            std::cref(camPos)
        ));
    }

    for(auto &t : threads) {
        if(t.joinable()) t.join();
    }
    std::cout << "Rendu terminé !" << std::endl;
}

void Image::saveImage(const std::string &filename){
    std::ofstream out(filename, std::ios::binary);

    // PPM
    out << "P6\n" << W << " " << H << "\n255\n";

    // ecrire chaque pixel dans le fichier ppm
    for (int j = 0; j < this->H; ++j) {
        for (int i = 0; i < this->W; ++i) {
            const Pixel &p = pixels[j*this->W + i];
            out.write((char*)&p.r, 1);
            out.write((char*)&p.g, 1);
            out.write((char*)&p.b, 1);
        }
    }

    out.close();
}




/*************************************************************************************************/
/*                                                                                               */
/*                                       Tetrade                                                 */
/*                                                                                               */
/*                                                                                               */
/*************************************************************************************************/


//old one
// Tetrade::Tetrade(const Vec4 &camPos) {
//     double f = 1.0 - (2.0 / camPos.r); //
//     double sinth = sin(camPos.theta);
//     if (std::abs(sinth) < 1e-12) sinth = (sinth >= 0 ? 1e-12 : -1e-12); // pour pas diviser par 0

//     this->e0 = new Vec4(0.0, 0.0, 0.0, 1.0 / sqrt(f));
//     this->e1 = new Vec4(sqrt(f), 0.0, 0.0, 0.0);
//     this->e2 = new Vec4(0.0, 1.0 / camPos.r, 0.0, 0.0);
//     this->e3 = new Vec4(0.0, 0.0, 1.0 / (camPos.r * sinth), 0.0);

// }

Tetrade::Tetrade(const Vec4 &camPos) {
    double f = 1.0 - (2.0 / camPos.r); //
    double sinth = sin(camPos.theta);
    if (std::abs(sinth) < 1e-12) sinth = (sinth >= 0 ? 1e-12 : -1e-12); // pour pas diviser par 0

    this->e0 = new Vec4(sqrt(f), 0.0, 0.0, 0.0);
    this->e1 = new Vec4(0.0, (1.0 / camPos.r), 0.0, 0.0);
    this->e2 = new Vec4(0.0, 0.0, 1.0 / (camPos.r * sinth), 0.0);
    this->e3 = new Vec4(0.0, 0.0, 0.0, 1.0 / sqrt(f));

}

Tetrade::~Tetrade(){

    delete(this->e0);
    delete(this->e1);
    delete(this->e2);
    delete(this->e3);

}