#pragma once

#include <vector>
#include <stdio.h>
#include <math.h>


struct Skybox {
    int width, height;
    std::vector<unsigned char> data;

    void load(const char* filename);
    void getColor(double theta, double phi, int &r, int &g, int &b) const;
};