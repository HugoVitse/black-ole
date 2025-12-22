#pragma once
#include "pixel.hpp"
#include "blackhole.hpp"
#include <string>
#include <thread>
#include <vector>

struct Image {

    std::vector<Pixel> pixels;
    int W,H;
    double h;
    static int nb_threads;
    Tetrade tetrade;
    std::vector<std::thread> threads;

    Image(int _W, int _H, double FOV, const Vec4 &camPos);
    ~Image() = default;
    void computeImage(const BlackHole &blackhole, const Vec4 &camPos);
    void computeRow(const BlackHole &blackhole, int start_y, int end_y, const Vec4 &camPos);
    void saveImage(const std::string &filename);

};