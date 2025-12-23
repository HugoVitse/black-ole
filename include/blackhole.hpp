#pragma once
#include "vectors.hpp"
#include <math.h>

struct BlackHole {
    double mass;
    double rs;
    double rmax;

    BlackHole(double _mass) : mass(_mass), rs(2*_mass), rmax(1100*_mass) {}
  

};