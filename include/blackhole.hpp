#pragma once
#include "vectors.hpp"
#include <math.h>

struct BlackHole {
    double mass;
    double rs;
    double rmax;

    BlackHole(double _mass) : mass(_mass), rs(2*_mass), rmax(1100*_mass) {}
    inline double christoffel(int mu, int alpha, int beta, const Vec4& x) const {
        switch(mu) {
            case 3: // t
                if (alpha == 3 && beta == 0) return mass / (x.r * (x.r - 2 * mass));
                if (alpha == 0 && beta == 3) return mass / (x.r * (x.r - 2 * mass));
                return 0;
            
            case 0: // r
                if (alpha == 3 && beta == 3) return (mass * (x.r - 2 * mass)) / std::pow(x.r, 3);
                if (alpha == 1 && beta == 1) return - (x.r - 2 * mass);
                if (alpha == 2 && beta == 2) return - (x.r - 2 * mass) * std::pow(std::sin(x.theta), 2);
                if (alpha == 0 && beta == 0) return - (mass / (x.r * (x.r - 2 * mass)));
                return 0;

            case 1: // theta
                if ((alpha == 0 && beta == 1) || (alpha == 1 && beta == 0)) return 1.0 / x.r;
                if (alpha == 2 && beta == 2) return - std::sin(x.theta) * std::cos(x.theta);
                return 0;

            case 2: // phi
                if ((alpha == 0 && beta == 2) || (alpha == 2 && beta == 0)) return 1.0 / x.r;
                if ((alpha == 1 && beta == 2) || (alpha == 2 && beta == 1)) {
                    double s = std::sin(x.theta);
                    // Si on est trop près du pôle, on neutralise la force centrifuge infinie
                    if (std::abs(s) < 1e-4) return 0.0; 
                    return std::cos(x.theta) / s;
                }

                return 0;

            default: return 0;
        }
    }

};