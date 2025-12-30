struct BlackHole {
    float mass;
    float rs;
    float rmax;
};


float christoffel(int mu, int alpha, int beta, vec4 x, BlackHole blackhole) {
    switch(mu) {
        case 3: // t
            if (alpha == 3 && beta == 0) return blackhole.mass / (x.x * (x.x - 2 * blackhole.mass));
            if (alpha == 0 && beta == 3) return blackhole.mass / (x.x * (x.x - 2 * blackhole.mass));
            return 0;
        
        case 0: // r
            if (alpha == 3 && beta == 3) return (blackhole.mass * (x.x - 2 * blackhole.mass)) / pow(x.x, 3);
            if (alpha == 1 && beta == 1) return - (x.x - 2 * blackhole.mass);
            if (alpha == 2 && beta == 2) return - (x.x - 2 * blackhole.mass) * pow(sin(x.y), 2);
            if (alpha == 0 && beta == 0) return - (blackhole.mass / (x.x * (x.x - 2 * blackhole.mass)));
            return 0;

        case 1: // theta
            if ((alpha == 0 && beta == 1) || (alpha == 1 && beta == 0)) return 1.0 / x.x;
            if (alpha == 2 && beta == 2) return - sin(x.y) * cos(x.y);
            return 0;

        case 2: // phi
            if ((alpha == 0 && beta == 2) || (alpha == 2 && beta == 0)) return 1.0 / x.x;
            if ((alpha == 1 && beta == 2) || (alpha == 2 && beta == 1)) {
                float s = sin(x.y);
                // Si on est trop près du pôle, on neutralise la force centrifuge infinie
                float epsilon = 1e-6; 
                float safe_sin = (abs(s) < epsilon) ? (s >= 0 ? epsilon: -epsilon) : s;
                return cos(x.y) / safe_sin;
            }

            return 0;

        default: return 0;
    }
};