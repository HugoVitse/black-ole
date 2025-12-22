#pragma once
#include "vectors.hpp"

struct Tetrade {
    Vec4 *e0, *e1, *e2, *e3;

    Tetrade(const Vec4 &camPos);
    ~Tetrade();

};
