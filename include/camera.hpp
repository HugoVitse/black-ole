#pragma once
#include "vectors.hpp"
#include "image.hpp"

struct Camera {
    double FOV;
    Vec4 &position;

    Camera(double _FOV , Vec4& pos) : FOV(_FOV), position(pos) {}
};