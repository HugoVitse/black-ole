#pragma once
#include "tetrade.hpp"
#include "vectors.hpp"
#include "photon.hpp"
#include "blackhole.hpp"
#include "skybox.hpp"


struct Pixel {
    char r,g,b;
    int i,j;

    Vec4 k_local;
    Vec4 k_global;

    Photon photon;

    
    Pixel(int _i, int _j, int W, int H, double h, Tetrade* tetrade, const Vec4 &camPos);
    Pixel();
    
    void castPhoton(const BlackHole &blackhole);
    void setColor(const BlackHole &blackhole, const Skybox &skybox);
};